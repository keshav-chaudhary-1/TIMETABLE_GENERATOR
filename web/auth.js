(function attachAuthModule(global) {
  const STORAGE_KEY = "timetablePortalSession";

  const ADMIN_CREDENTIALS = {
    userId: "Geu",
    password: "Geu-admin",
  };

  // The provided examples are 9-digit IDs, so the student flow validates
  // against those registered IDs exactly.
  const STUDENT_IDS = new Set([
    "240211785",
    "240211789",
    "240260491",
  ]);

  const STUDENT_PASSWORD = "Timer@123";

  function normalizeUserId(value) {
    return String(value || "").trim();
  }

  function isStudentId(userId) {
    return /^\d+$/.test(userId);
  }

  function createSession(payload) {
    return {
      role: payload.role,
      userId: payload.userId,
      displayName: payload.displayName,
      loggedInAt: new Date().toISOString(),
    };
  }

  function authenticate(rawUserId, rawPassword) {
    const userId = normalizeUserId(rawUserId);
    const password = String(rawPassword || "");

    if (!userId || !password) {
      return {
        ok: false,
        message: "Enter both user ID and password.",
      };
    }

    if (userId === ADMIN_CREDENTIALS.userId) {
      if (password !== ADMIN_CREDENTIALS.password) {
        return {
          ok: false,
          message: "Admin password is incorrect.",
        };
      }

      return {
        ok: true,
        session: createSession({
          role: "admin",
          userId,
          displayName: "GEU Admin",
        }),
      };
    }

    if (!isStudentId(userId)) {
      return {
        ok: false,
        message: "Use admin ID `Geu` or a registered numeric student ID.",
      };
    }

    if (!STUDENT_IDS.has(userId)) {
      return {
        ok: false,
        message: "Student ID is not registered for this portal.",
      };
    }

    if (password !== STUDENT_PASSWORD) {
      return {
        ok: false,
        message: "Student password is incorrect.",
      };
    }

    return {
      ok: true,
      session: createSession({
        role: "student",
        userId,
        displayName: `Student ${userId}`,
      }),
    };
  }

  function saveSession(session) {
    localStorage.setItem(STORAGE_KEY, JSON.stringify(session));
    return session;
  }

  function getSession() {
    const raw = localStorage.getItem(STORAGE_KEY);
    if (!raw) return null;

    try {
      const parsed = JSON.parse(raw);
      if (!parsed || !parsed.role || !parsed.userId) return null;
      return parsed;
    } catch (error) {
      return null;
    }
  }

  function clearSession() {
    localStorage.removeItem(STORAGE_KEY);
  }

  global.AuthModule = {
    authenticate,
    clearSession,
    getSession,
    saveSession,
  };
})(window);
