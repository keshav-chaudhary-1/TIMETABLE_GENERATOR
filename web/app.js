function redirectTo(path) {
  window.location.href = path;
}

function ensureAuth() {
  const session = window.AuthModule?.getSession?.();
  if (!session) {
    redirectTo("index.html");
    return null;
  }
  return session;
}

function getQueryParam(name) {
  return new URLSearchParams(window.location.search).get(name);
}

async function getText(url) {
  const response = await fetch(url, { cache: "no-store" });
  if (!response.ok) {
    throw new Error(`Failed to load ${url}`);
  }
  return response.text();
}

function parseCsv(text) {
  const lines = text
    .split(/\r?\n/)
    .map((line) => line.trim())
    .filter(Boolean);

  if (lines.length === 0) return [];

  const headers = lines[0].split(",").map((part) => part.trim());
  return lines.slice(1).map((line) => {
    const values = line.split(",").map((part) => part.trim());
    return headers.reduce((row, header, index) => {
      row[header] = values[index] || "";
      return row;
    }, {});
  });
}

async function loadCsv(paths) {
  let lastError = null;

  for (const path of paths) {
    try {
      const text = await getText(path);
      return parseCsv(text);
    } catch (error) {
      lastError = error;
    }
  }

  throw lastError || new Error("Unable to load CSV file.");
}

function uniqueSorted(values) {
  return [...new Set(values)].sort((a, b) => a.localeCompare(b));
}

const dayOrder = ["Mon", "Tue", "Wed", "Thu", "Fri", "Sat"];
const dayLabels = {
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

const defaultPeriods = [
  { value: 0, label: "8:00-8:55" },
  { value: 1, label: "9:00-9:55" },
  { value: 2, label: "9:55-10:50" },
  { value: 3, label: "11:10-12:05" },
  { value: 4, label: "12:05-1:00" },
  { value: 5, label: "1:00-1:55" },
  { value: 6, label: "1:55-2:50" },
  { value: 7, label: "3:10-4:05" },
];

const fileSet = {
  subjects: ["../data/subjects.csv"],
  teachers: ["../data/teachers.csv"],
  rooms: ["../data/rooms.csv"],
  schedule: ["../output_latest_schedule.csv", "../output_v1_schedule.csv"],
  teacherSchedule: ["../output_latest_teachers.csv", "../output_v1_teachers.csv"],
  section: (key) => [`../output_latest_section_${key}.csv`, `../output_v1_section_${key}.csv`],
};

const subjectThemeMap = {
  "TCS-408": {
    bg: "linear-gradient(180deg, rgba(133, 245, 160, 0.82), rgba(91, 220, 124, 0.72))",
    border: "rgba(58, 154, 83, 0.55)",
    shadow: "rgba(73, 183, 101, 0.25)",
  },
  "PCS-408": {
    bg: "linear-gradient(180deg, rgba(121, 230, 159, 0.82), rgba(74, 206, 120, 0.72))",
    border: "rgba(52, 145, 83, 0.52)",
    shadow: "rgba(69, 178, 103, 0.24)",
  },
  "TCS-402": {
    bg: "linear-gradient(180deg, rgba(255, 244, 125, 0.88), rgba(247, 226, 87, 0.74))",
    border: "rgba(201, 173, 33, 0.52)",
    shadow: "rgba(223, 198, 67, 0.24)",
  },
  "TCS-403": {
    bg: "linear-gradient(180deg, rgba(245, 123, 255, 0.84), rgba(225, 85, 232, 0.72))",
    border: "rgba(171, 56, 177, 0.54)",
    shadow: "rgba(207, 95, 213, 0.24)",
  },
  "PCS-403": {
    bg: "linear-gradient(180deg, rgba(233, 124, 255, 0.82), rgba(210, 87, 230, 0.72))",
    border: "rgba(164, 64, 182, 0.52)",
    shadow: "rgba(203, 108, 214, 0.22)",
  },
  "TCS-409": {
    bg: "linear-gradient(180deg, rgba(124, 241, 248, 0.84), rgba(88, 221, 232, 0.72))",
    border: "rgba(53, 161, 172, 0.52)",
    shadow: "rgba(84, 203, 214, 0.24)",
  },
  "PCS-409": {
    bg: "linear-gradient(180deg, rgba(118, 231, 245, 0.82), rgba(79, 205, 223, 0.72))",
    border: "rgba(53, 153, 168, 0.5)",
    shadow: "rgba(85, 196, 209, 0.22)",
  },
};

const fallbackThemes = [
  {
    bg: "linear-gradient(180deg, rgba(255, 210, 210, 0.9), rgba(249, 170, 170, 0.76))",
    border: "rgba(205, 108, 108, 0.5)",
    shadow: "rgba(228, 133, 133, 0.24)",
  },
  {
    bg: "linear-gradient(180deg, rgba(214, 248, 197, 0.88), rgba(171, 230, 145, 0.74))",
    border: "rgba(112, 178, 88, 0.5)",
    shadow: "rgba(155, 210, 126, 0.24)",
  },
  {
    bg: "linear-gradient(180deg, rgba(255, 236, 194, 0.9), rgba(244, 209, 142, 0.74))",
    border: "rgba(194, 152, 71, 0.48)",
    shadow: "rgba(220, 181, 102, 0.24)",
  },
  {
    bg: "linear-gradient(180deg, rgba(220, 229, 255, 0.9), rgba(181, 199, 246, 0.76))",
    border: "rgba(103, 124, 196, 0.46)",
    shadow: "rgba(145, 163, 224, 0.24)",
  },
];

function hashString(value) {
  let hash = 0;
  for (let index = 0; index < value.length; index += 1) {
    hash = (hash * 31 + value.charCodeAt(index)) >>> 0;
  }
  return hash;
}

function getSubjectTheme(subjectCode) {
  if (subjectThemeMap[subjectCode]) {
    return subjectThemeMap[subjectCode];
  }

  const hashed = hashString(subjectCode || "subject");
  return fallbackThemes[hashed % fallbackThemes.length];
}

function buildGrid(records, type) {
  const periodMap = new Map();
  const cells = new Map();

  records.forEach((record) => {
    const day = dayLabels[record.Day] ?? dayLabels[Number(record.Day)] ?? record.Day;
    const period = Number(record.Period);
    const periodKey = Number.isNaN(period) ? record.Period : period;
    periodMap.set(periodKey, record.Time || `Period ${Number(periodKey) + 1}`);

    const key = `${day}-${periodKey}`;
    const current = cells.get(key) || [];
    current.push(record);
    cells.set(key, current);
  });

  const periods =
    periodMap.size > 0
      ? [...periodMap.entries()]
          .sort((a, b) => Number(a[0]) - Number(b[0]))
          .map(([value, label]) => ({
            value,
            label,
            display: `P${Number(value) + 1}`,
          }))
      : defaultPeriods.map((period) => ({
          ...period,
          display: `P${period.value + 1}`,
        }));

  const rows = dayOrder.map((day) => ({
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

async function loadSummaryData() {
  const [subjects, teachers, rooms, schedule] = await Promise.all([
    loadCsv(fileSet.subjects),
    loadCsv(fileSet.teachers),
    loadCsv(fileSet.rooms),
    loadCsv(fileSet.schedule),
  ]);

  return {
    sections: uniqueSorted(subjects.map((subject) => subject.section)),
    teachers: uniqueSorted(teachers.map((teacher) => teacher.name)),
    rooms: uniqueSorted(rooms.map((room) => room.name)),
    stats: {
      sections: uniqueSorted(subjects.map((subject) => subject.section)).length,
      teachers: teachers.length,
      rooms: rooms.length,
      lectureSlots: schedule.length,
    },
    source: schedule.some((entry) => entry.Slot) ? "Live export files" : "Static export files",
  };
}

async function loadTimetableData(type, key) {
  if (type === "section") {
    const records = await loadCsv(fileSet.section(key));
    return {
      label: `Section ${key}`,
      records,
      grid: buildGrid(records, "section"),
    };
  }

  if (type === "teacher") {
    const all = await loadCsv(fileSet.teacherSchedule);
    const records = all.filter((row) => row.Teacher === key);
    return {
      label: key,
      records,
      grid: buildGrid(records, "teacher"),
    };
  }

  const all = await loadCsv(fileSet.schedule);
  const records = all.filter((row) => row.Room === key);
  return {
    label: key,
    records,
    grid: buildGrid(records, "room"),
  };
}

function cardHtml(title, value) {
  return `
    <article class="stat-card">
      <strong>${value}</strong>
      <span>${title}</span>
    </article>
  `;
}

function prettifyRole(role) {
  if (role === "admin") return "Admin";
  if (role === "student") return "Student";
  return "User";
}

function renderSessionChrome(session) {
  const roleNode = document.getElementById("sessionRole");
  const userNode = document.getElementById("sessionUser");
  const logoutBtn = document.getElementById("logoutBtn");

  if (roleNode) {
    roleNode.textContent = prettifyRole(session.role);
  }

  if (userNode) {
    userNode.textContent = session.displayName || session.userId;
  }

  if (logoutBtn) {
    logoutBtn.addEventListener("click", () => {
      window.AuthModule.clearSession();
      redirectTo("index.html");
    });
  }
}

function showAuthMessage(node, message, kind) {
  if (!node) return;
  node.textContent = message;
  node.classList.remove("is-success");
  node.classList.add("is-visible");
  if (kind === "success") {
    node.classList.add("is-success");
  }
}

function renderTable(tableElement, payload) {
  const head = tableElement.querySelector("thead");
  const body = tableElement.querySelector("tbody");
  const periods = payload.grid.periods;
  const rows = payload.grid.rows;

  head.innerHTML = `
    <tr>
      <th>Day</th>
      ${periods
        .map(
          (period) =>
            `<th>${period.display}<br /><small>${period.label}</small></th>`
        )
        .join("")}
    </tr>
  `;

  body.innerHTML = rows
    .map((row) => {
      const cells = row.cells
        .map((entries) => {
          if (entries.length === 0) {
            return `<td><div class="slot-empty">Free</div></td>`;
          }

          return `
            <td>
              ${entries
                .map((entry) => {
                  const theme = getSubjectTheme(entry.title);
                  return `
                    <div
                      class="slot-card"
                      style="--subject-bg:${theme.bg}; --subject-border:${theme.border}; --subject-shadow:${theme.shadow};"
                    >
                      <div class="slot-title">${entry.title}</div>
                      <div class="slot-subtitle">${entry.subtitle}</div>
                      <div class="slot-meta">${entry.meta}</div>
                    </div>
                  `
                })
                .join("")}
            </td>
          `;
        })
        .join("");

      return `<tr><th>${row.day}</th>${cells}</tr>`;
    })
    .join("");
}

async function initLoginPage() {
  const form = document.getElementById("loginForm");
  if (!form) return;
  const authMessage = document.getElementById("authMessage");

  const activeSession = window.AuthModule.getSession();
  if (activeSession) {
    redirectTo("dashboard.html");
    return;
  }

  form.addEventListener("submit", (event) => {
    event.preventDefault();
    const userId = document.getElementById("userId").value.trim();
    const password = document.getElementById("password").value.trim();

    const result = window.AuthModule.authenticate(userId, password);
    if (!result.ok) {
      showAuthMessage(authMessage, result.message, "error");
      return;
    }

    showAuthMessage(
      authMessage,
      `${prettifyRole(result.session.role)} authentication successful.`,
      "success"
    );
    window.AuthModule.saveSession(result.session);
    redirectTo("dashboard.html");
  });
}

async function initDashboardPage() {
  const session = ensureAuth();
  if (!session) return;

  const summaryStats = document.getElementById("summaryStats");
  const regenerateBtn = document.getElementById("regenerateBtn");
  const cards = document.querySelectorAll(".mode-card");
  renderSessionChrome(session);

  const loadSummary = async () => {
    const summary = await loadSummaryData();
    summaryStats.innerHTML = [
      cardHtml("Sections", summary.stats.sections),
      cardHtml("Teachers", summary.stats.teachers),
      cardHtml("Rooms", summary.stats.rooms),
      cardHtml("Generated entries", summary.stats.lectureSlots),
    ].join("");
  };

  await loadSummary();

  regenerateBtn.addEventListener("click", async () => {
    regenerateBtn.disabled = true;
    regenerateBtn.textContent = "Reloading...";

    try {
      await loadSummary();
      regenerateBtn.textContent = "Files Reloaded";
      setTimeout(() => {
        regenerateBtn.textContent = "Reload Export Files";
      }, 1200);
    } catch (error) {
      regenerateBtn.textContent = "Retry Reload";
      alert(error.message);
    } finally {
      regenerateBtn.disabled = false;
    }
  });

  cards.forEach((card) => {
    card.addEventListener("click", () => {
      const type = card.dataset.target;
      redirectTo(`viewer.html?type=${encodeURIComponent(type)}`);
    });
  });
}

async function initViewerPage() {
  const session = ensureAuth();
  if (!session) return;

  const requestedType = getQueryParam("type") || "section";
  const viewerTitle = document.getElementById("viewerTitle");
  const viewerKicker = document.getElementById("viewerKicker");
  const versionPill = document.getElementById("versionPill");
  const tableTitle = document.getElementById("tableTitle");
  const entryCount = document.getElementById("entryCount");
  const entitySelect = document.getElementById("entitySelect");
  const table = document.getElementById("timetableTable");
  const refreshViewerBtn = document.getElementById("refreshViewerBtn");

  const labels = {
    section: {
      page: "Section Wise Timetable",
      kicker: "Student schedule view",
      list: "sections",
    },
    teacher: {
      page: "Teacher Wise Timetable",
      kicker: "Faculty schedule view",
      list: "teachers",
    },
    room: {
      page: "Room Wise Timetable",
      kicker: "Room occupancy view",
      list: "rooms",
    },
  };

  const type = labels[requestedType] ? requestedType : "section";
  const current = labels[type];
  viewerTitle.textContent = current.page;
  viewerKicker.textContent = current.kicker;
  renderSessionChrome(session);

  const loadSummary = async () => loadSummaryData();

  const loadTimetable = async () => {
    const key = entitySelect.value;
    if (!key) return;

    const payload = await loadTimetableData(type, key);

    versionPill.textContent = "Static file mode";
    tableTitle.textContent = payload.label;
    entryCount.textContent = `${payload.records.length} entries`;
    renderTable(table, payload);
  };

  const buildOptions = (items) => {
    entitySelect.innerHTML = items
      .map((item) => `<option value="${item}">${item}</option>`)
      .join("");
  };

  const summary = await loadSummary();
  buildOptions(summary[current.list]);
  await loadTimetable();

  entitySelect.addEventListener("change", loadTimetable);

  refreshViewerBtn.addEventListener("click", async () => {
    refreshViewerBtn.disabled = true;
    refreshViewerBtn.textContent = "Reloading...";

    try {
      await loadTimetable();
      refreshViewerBtn.textContent = "Data Reloaded";
      setTimeout(() => {
        refreshViewerBtn.textContent = "Reload Data";
      }, 1200);
    } catch (error) {
      refreshViewerBtn.textContent = "Retry Reload";
      alert(error.message);
    } finally {
      refreshViewerBtn.disabled = false;
    }
  });
}

async function boot() {
  const page = document.body.dataset.page;

  if (page === "login") await initLoginPage();
  if (page === "dashboard") await initDashboardPage();
  if (page === "viewer") await initViewerPage();
}

boot().catch((error) => {
  console.error(error);
  alert(error.message);
});
