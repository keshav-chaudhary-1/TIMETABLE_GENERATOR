const http = require("http");
const fs = require("fs");
const path = require("path");
const { spawn } = require("child_process");

const ROOT_DIR = __dirname;
const WEB_DIR = path.join(ROOT_DIR, "web");
const PUBLIC_DIR = path.join(ROOT_DIR, "public");
const PORT = process.env.PORT || 3000;

const DAY_ORDER = ["Mon", "Tue", "Wed", "Thu", "Fri", "Sat"];
const DEFAULT_PERIODS = [
  { value: 0, label: "8:00-8:55" },
  { value: 1, label: "9:00-9:55" },
  { value: 2, label: "9:55-10:50" },
  { value: 3, label: "11:10-12:05" },
  { value: 4, label: "12:05-1:00" },
  { value: 5, label: "1:00-1:55" },
  { value: 6, label: "1:55-2:50" },
  { value: 7, label: "3:10-4:05" },
];
const DAY_LABELS = {
  0: "Mon",
  1: "Tue",
  2: "Wed",
  3: "Thu",
  4: "Fri",
  5: "Sat",
  Mon: "Mon",
  Tue: "Tue",
  Wed: "Wed",
  Thu: "Thu",
  Fri: "Fri",
  Sat: "Sat",
  Monday: "Mon",
  Tuesday: "Tue",
  Wednesday: "Wed",
  Thursday: "Thu",
  Friday: "Fri",
  Saturday: "Sat",
};

const MIME_TYPES = {
  ".css": "text/css; charset=utf-8",
  ".html": "text/html; charset=utf-8",
  ".js": "application/javascript; charset=utf-8",
  ".json": "application/json; charset=utf-8",
  ".png": "image/png",
  ".svg": "image/svg+xml",
  ".jpg": "image/jpeg",
  ".jpeg": "image/jpeg",
  ".ico": "image/x-icon",
};

function sendJson(res, statusCode, payload) {
  res.writeHead(statusCode, { "Content-Type": MIME_TYPES[".json"] });
  res.end(JSON.stringify(payload));
}

function sendText(res, statusCode, text) {
  res.writeHead(statusCode, { "Content-Type": "text/plain; charset=utf-8" });
  res.end(text);
}

function safeFile(baseDir, relativePath) {
  const finalPath = path.normalize(path.join(baseDir, relativePath));
  return finalPath.startsWith(baseDir) ? finalPath : null;
}

function serveFile(res, filePath) {
  fs.readFile(filePath, (error, content) => {
    if (error) {
      sendText(res, 404, "Not found");
      return;
    }

    const ext = path.extname(filePath).toLowerCase();
    res.writeHead(200, {
      "Content-Type": MIME_TYPES[ext] || "application/octet-stream",
    });
    res.end(content);
  });
}

function parseCsv(content) {
  const lines = content
    .split(/\r?\n/)
    .map((line) => line.trim())
    .filter(Boolean);

  if (lines.length === 0) return [];

  const headers = lines[0].split(",").map((part) => part.trim());
  return lines.slice(1).map((line) => {
    const values = line.split(",").map((part) => part.trim());
    const row = {};
    headers.forEach((header, index) => {
      row[header] = values[index] || "";
    });
    return row;
  });
}

function readCsv(filePath) {
  return parseCsv(fs.readFileSync(filePath, "utf8"));
}

function latestVersion() {
  const versionPattern = /^output_v(\d+)_schedule\.csv$/;
  let best = 0;

  for (const file of fs.readdirSync(ROOT_DIR)) {
    const match = file.match(versionPattern);
    if (match) best = Math.max(best, Number(match[1]));
  }

  return best;
}

function runProcess(command, args, options = {}) {
  return new Promise((resolve, reject) => {
    const child = spawn(command, args, {
      cwd: ROOT_DIR,
      stdio: ["pipe", "pipe", "pipe"],
      ...options,
    });

    let stdout = "";
    let stderr = "";

    child.stdout.on("data", (chunk) => {
      stdout += chunk.toString();
    });

    child.stderr.on("data", (chunk) => {
      stderr += chunk.toString();
    });

    child.on("error", reject);

    child.on("close", (code) => {
      if (code !== 0) {
        reject(new Error(`${command} ${args.join(" ")} failed\n${stderr || stdout}`));
        return;
      }
      resolve({ stdout, stderr });
    });
  });
}

async function regenerateExports() {
  await runProcess("make", ["all"]);

  await new Promise((resolve, reject) => {
    const child = spawn("./timetable_cse2yr", [], {
      cwd: ROOT_DIR,
      stdio: ["pipe", "pipe", "pipe"],
    });

    let stderr = "";
    let stdout = "";

    child.stdout.on("data", (chunk) => {
      stdout += chunk.toString();
    });

    child.stderr.on("data", (chunk) => {
      stderr += chunk.toString();
    });

    child.on("error", reject);

    child.on("close", (code) => {
      if (code !== 0) {
        reject(new Error(`timetable_cse2yr failed\n${stderr || stdout}`));
        return;
      }
      resolve();
    });

    child.stdin.write("5\nq\n");
    child.stdin.end();
  });

  return latestVersion();
}

async function ensureExports() {
  const version = latestVersion();
  if (version > 0) return version;
  return regenerateExports();
}

function buildGrid(records, type) {
  const periodMap = new Map();
  const cells = new Map();

  for (const record of records) {
    const day = DAY_LABELS[record.Day] ?? DAY_LABELS[Number(record.Day)] ?? record.Day;
    const periodValue = Number(record.Period);
    const periodKey = Number.isNaN(periodValue) ? record.Period : periodValue;
    const timeLabel = record.Time || `Period ${periodKey + 1}`;
    periodMap.set(periodKey, timeLabel);

    const cellKey = `${day}-${periodKey}`;
    const current = cells.get(cellKey) || [];
    current.push(record);
    cells.set(cellKey, current);
  }

  const periods = [...periodMap.entries()]
    .sort((a, b) => Number(a[0]) - Number(b[0]))
    .map(([value, label]) => ({
      value,
      label,
      display: `P${Number(value) + 1}`,
    }));

  if (periods.length === 0) {
    for (const period of DEFAULT_PERIODS) {
      periods.push({
        ...period,
        display: `P${period.value + 1}`,
      });
    }
  }

  const rows = DAY_ORDER.map((day) => ({
    day,
    cells: periods.map((period) => {
      const key = `${day}-${period.value}`;
      const matches = cells.get(key) || [];
      return matches.map((entry) => {
        if (type === "section") {
          return {
            title: entry.SubjectCode,
            subtitle: entry.SubjectName,
            meta: `${entry.Teachers} • ${entry.Room}`,
          };
        }

        if (type === "teacher") {
          return {
            title: entry.SubjectCode,
            subtitle: `Section ${entry.Section}`,
            meta: entry.Room,
          };
        }

        return {
          title: entry.SubjectCode,
          subtitle: `Section ${entry.Section}`,
          meta: entry.Teachers,
        };
      });
    }),
  }));

  return { periods, rows };
}

function readSummary(version) {
  const subjects = readCsv(path.join(ROOT_DIR, "data", "subjects.csv"));
  const teachers = readCsv(path.join(ROOT_DIR, "data", "teachers.csv"));
  const roomsCsv = readCsv(path.join(ROOT_DIR, "data", "rooms.csv"));
  const schedule = readCsv(path.join(ROOT_DIR, `output_v${version}_schedule.csv`));

  const sections = [...new Set(subjects.map((subject) => subject.section))].sort();
  const teacherNames = teachers.map((teacher) => teacher.name).sort((a, b) => a.localeCompare(b));
  const rooms = roomsCsv.map((room) => room.name).sort((a, b) => a.localeCompare(b));

  return {
    version,
    sections,
    teachers: teacherNames,
    rooms,
    stats: {
      sections: sections.length,
      teachers: teacherNames.length,
      rooms: rooms.length,
      lectureSlots: schedule.length,
    },
  };
}

function sectionData(version, sectionKey) {
  const filePath = path.join(ROOT_DIR, `output_v${version}_section_${sectionKey}.csv`);
  const records = readCsv(filePath);

  return {
    label: `Section ${sectionKey}`,
    type: "section",
    records,
    grid: buildGrid(records, "section"),
  };
}

function teacherData(version, teacherKey) {
  const filePath = path.join(ROOT_DIR, `output_v${version}_teachers.csv`);
  const records = readCsv(filePath).filter((record) => record.Teacher === teacherKey);

  return {
    label: teacherKey,
    type: "teacher",
    records,
    grid: buildGrid(records, "teacher"),
  };
}

function roomData(version, roomKey) {
  const filePath = path.join(ROOT_DIR, `output_v${version}_schedule.csv`);
  const records = readCsv(filePath).filter((record) => record.Room === roomKey);

  return {
    label: roomKey,
    type: "room",
    records,
    grid: buildGrid(records, "room"),
  };
}

async function handleApi(req, res, pathname, searchParams) {
  try {
    if (pathname === "/api/regenerate" && req.method === "POST") {
      const version = await regenerateExports();
      sendJson(res, 200, {
        ok: true,
        summary: readSummary(version),
      });
      return;
    }

    const version = await ensureExports();

    if (pathname === "/api/summary") {
      sendJson(res, 200, readSummary(version));
      return;
    }

    if (pathname === "/api/timetable") {
      const type = searchParams.get("type");
      const key = searchParams.get("key");

      if (!type || !key) {
        sendJson(res, 400, { error: "Missing timetable type or key." });
        return;
      }

      let payload;
      if (type === "section") payload = sectionData(version, key);
      else if (type === "teacher") payload = teacherData(version, key);
      else if (type === "room") payload = roomData(version, key);
      else {
        sendJson(res, 400, { error: "Unsupported timetable type." });
        return;
      }

      sendJson(res, 200, {
        version,
        ...payload,
      });
      return;
    }

    sendJson(res, 404, { error: "API route not found." });
  } catch (error) {
    sendJson(res, 500, {
      error: error.message,
    });
  }
}

function handleStatic(res, pathname) {
  const pageMap = {
    "/": "index.html",
    "/dashboard": "dashboard.html",
    "/viewer": "viewer.html",
  };

  if (pageMap[pathname]) {
    serveFile(res, path.join(WEB_DIR, pageMap[pathname]));
    return true;
  }

  if (pathname.startsWith("/assets/")) {
    const assetPath = safeFile(PUBLIC_DIR, pathname.replace("/assets/", "assets/"));
    if (!assetPath) return false;
    serveFile(res, assetPath);
    return true;
  }

  const webFile = safeFile(WEB_DIR, pathname.slice(1));
  if (webFile && fs.existsSync(webFile) && fs.statSync(webFile).isFile()) {
    serveFile(res, webFile);
    return true;
  }

  const rootFile = safeFile(ROOT_DIR, pathname.slice(1));
  if (rootFile && fs.existsSync(rootFile) && fs.statSync(rootFile).isFile()) {
    serveFile(res, rootFile);
    return true;
  }

  return false;
}

const server = http.createServer(async (req, res) => {
  const baseUrl = `http://${req.headers.host || `localhost:${PORT}`}`;
  const requestUrl = new URL(req.url, baseUrl);
  const pathname = requestUrl.pathname;

  if (pathname.startsWith("/api/")) {
    await handleApi(req, res, pathname, requestUrl.searchParams);
    return;
  }

  if (handleStatic(res, pathname)) return;

  sendText(res, 404, "Not found");
});

server.listen(PORT, "127.0.0.1", () => {
  console.log(`Timetable web app running on http://127.0.0.1:${PORT}`);
});
