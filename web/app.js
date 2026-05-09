const $ = (id) => document.getElementById(id);

const statusText = $("statusText");
const runtimeNote = $("runtimeNote");
const lastUpdateText = $("lastUpdateText");
const pageTitle = $("pageTitle");
const pageSubtitle = $("pageSubtitle");

const refreshBtn = $("refreshBtn");
const autoRefreshBtn = $("autoRefreshBtn");
const themeToggleBtn = $("themeToggleBtn");
const clearOpenBtn = $("clearOpenBtn");
const limitInput = $("limitInput");

const topTotalEvents = $("topTotalEvents");
const topHotAlerts = $("topHotAlerts");

const kpiEvents = $("kpiEvents");
const kpiAlerts = $("kpiAlerts");
const kpiHotAlerts = $("kpiHotAlerts");
const kpiRules = $("kpiRules");

const eventsTimeline = $("eventsTimeline");
const severityBars = $("severityBars");
const sourceBars = $("sourceBars");

const dashboardEvents = $("dashboardEvents");
const dashboardAlerts = $("dashboardAlerts");

const eventSearchInput = $("eventSearchInput");
const clearEventFiltersBtn = $("clearEventFiltersBtn");

const alertSearchInput = $("alertSearchInput");
const clearAlertFiltersBtn = $("clearAlertFiltersBtn");

const eventsList = $("eventsList");
const alertsList = $("alertsList");
const rulesList = $("rulesList");

const eventsCount = $("eventsCount");
const alertsCount = $("alertsCount");
const rulesCount = $("rulesCount");

const ruleMessage = $("ruleMessage");
const ruleFormTitle = $("ruleFormTitle");
const ruleIdInput = $("ruleIdInput");
const ruleEnabledInput = $("ruleEnabledInput");
const ruleTypeInput = $("ruleTypeInput");
const ruleSeverityInput = $("ruleSeverityInput");
const ruleEventTypesInput = $("ruleEventTypesInput");
const ruleSourceTypesInput = $("ruleSourceTypesInput");
const ruleGroupByInput = $("ruleGroupByInput");
const ruleThresholdInput = $("ruleThresholdInput");
const ruleWindowInput = $("ruleWindowInput");
const ruleSuppressInput = $("ruleSuppressInput");
const ruleTitleInput = $("ruleTitleInput");
const ruleDescriptionInput = $("ruleDescriptionInput");

const conditionFieldInput = $("conditionFieldInput");
const conditionOpInput = $("conditionOpInput");
const conditionValueInput = $("conditionValueInput");
const conditionValuesInput = $("conditionValuesInput");
const addConditionBtn = $("addConditionBtn");
const conditionsList = $("conditionsList");

const createRuleBtn = $("createRuleBtn");
const updateRuleBtn = $("updateRuleBtn");
const clearRuleFormBtn = $("clearRuleFormBtn");

let stats = {};
let allEvents = [];
let allAlerts = [];
let allRules = [];
let currentConditions = [];
let editingRuleId = "";

let autoRefreshEnabled = false;
let autoRefreshTimer = null;
let currentTheme = localStorage.getItem("mini_siem_theme") || "horror";

const openedPanels = new Set();
const eventPools = {};
const alertPools = {};

const AUTO_REFRESH_MS = 5000;

const tabMeta = {
  dashboard: {
    title: "Threat Dashboard",
    subtitle: "Manual investigation console for events, alerts and detection rules."
  },
  events: {
    title: "Events Investigation",
    subtitle: "KQL-like search with Raw, Fields and Packet inspection."
  },
  alerts: {
    title: "Alert Investigation",
    subtitle: "KQL-like alert search with Description, Source raw and Packet inspection."
  },
  rules: {
    title: "Detection Rules",
    subtitle: "Manage JSON-based SIEM rules from the browser."
  }
};

function escapeHtml(value) {
  return String(value ?? "")
    .replaceAll("&", "&amp;")
    .replaceAll("<", "&lt;")
    .replaceAll(">", "&gt;")
    .replaceAll('"', "&quot;")
    .replaceAll("'", "&#039;");
}

function escapeAttr(value) {
  return escapeHtml(value).replaceAll("`", "&#096;");
}

function lower(value) {
  return String(value ?? "").toLowerCase();
}

function formatNumber(value) {
  return Number(value || 0).toLocaleString();
}

function nowTime() {
  return new Date().toLocaleTimeString();
}

function setStatus(text) {
  statusText.textContent = text;
}

function touchUpdateTime() {
  lastUpdateText.textContent = nowTime();
}

function getLimit() {
  const value = Number(limitInput.value) || 250;
  return Math.max(10, Math.min(1000, value));
}

function severityClass(value) {
  const s = lower(value || "info");

  if (s === "critical") return "critical";
  if (s === "high") return "high";
  if (s === "medium") return "medium";
  if (s === "low") return "low";

  return "info";
}

function splitCsv(value) {
  return String(value || "")
    .split(",")
    .map((item) => item.trim())
    .filter(Boolean);
}

function joinCsv(values) {
  if (!Array.isArray(values)) return "";
  return values.join(", ");
}

function objectText(value) {
  try {
    return JSON.stringify(value ?? {});
  } catch {
    return String(value ?? "");
  }
}

function prettyJson(value) {
  try {
    return JSON.stringify(value ?? {}, null, 2);
  } catch {
    return "{}";
  }
}

function compactValue(value) {
  if (value === null || value === undefined || value === "") return "—";
  if (typeof value === "object") return JSON.stringify(value);
  return String(value);
}

async function fetchJson(url, options = {}) {
  const response = await fetch(url, options);
  const text = await response.text();

  let data = {};
  try {
    data = text ? JSON.parse(text) : {};
  } catch {
    data = { raw: text };
  }

  if (!response.ok) {
    throw new Error(data.message || data.raw || `HTTP ${response.status}`);
  }

  return data;
}

function rawEventText(item) {
  const ev = item.event || {};

  return (
    ev.raw ||
    ev.message ||
    ev.description ||
    item.description ||
    "No raw log line available"
  );
}

function rawAlertText(item) {
  const alert = item.alert || {};

  return (
    alert.raw ||
    alert.message ||
    alert.source_raw ||
    alert.description ||
    item.description ||
    "No raw source log available"
  );
}

function detailBox(id) {
  const open = openedPanels.has(id);
  return `<div id="${escapeAttr(id)}" class="detail-box ${open ? "open" : ""}"></div>`;
}

function detailPre(value) {
  return `<pre class="detail-pre">${escapeHtml(value)}</pre>`;
}

function detailFields(fields) {
  return `
    <div class="detail-fields">
      ${fields.map(([label, value]) => `
        <div class="detail-field">
          <span>${escapeHtml(label)}</span>
          <strong>${escapeHtml(compactValue(value))}</strong>
        </div>
      `).join("")}
    </div>
  `;
}

function setPanelContent(id, html) {
  const el = $(id);
  if (!el) return;

  el.innerHTML = html;
  el.classList.add("open");
  openedPanels.add(id);
}

function closePanel(id) {
  const el = $(id);
  if (!el) return;

  el.classList.remove("open");
  el.innerHTML = "";
  delete el.dataset.mode;
  openedPanels.delete(id);
}

function closeAllPanels() {
  openedPanels.clear();

  document.querySelectorAll(".detail-box").forEach((box) => {
    box.classList.remove("open");
    box.innerHTML = "";
    delete box.dataset.mode;
  });
}

function toggleEventPanel(scope, index, mode) {
  const item = eventPools[scope]?.[index];
  if (!item) return;

  const ev = item.event || {};
  const id = `${scope}-event-panel-${index}`;
  const el = $(id);

  if (openedPanels.has(id) && el?.dataset.mode === mode) {
    closePanel(id);
    return;
  }

  if (el) el.dataset.mode = mode;

  if (mode === "raw") {
    setPanelContent(id, `
      <div class="detail-title">Raw log line</div>
      ${detailPre(rawEventText(item))}
    `);
    return;
  }

  if (mode === "fields") {
    setPanelContent(id, `
      <div class="detail-title">Normalized fields</div>
      ${detailFields([
        ["DB ID", item.id],
        ["Event UUID", item.event_id || ev.event_id],
        ["Timestamp", item.ts || ev.ts],
        ["Received at", item.received_at || ev.received_at],
        ["Host", item.host || ev.host],
        ["Event type", item.event_type || ev.event_type],
        ["Source", item.source || ev.source],
        ["Source type", item.source_type || ev.source_type],
        ["Severity", item.severity || ev.severity],
        ["User", ev.user],
        ["Source IP", ev.src_ip],
        ["Port", ev.port],
        ["Program", ev.program || ev.process || ev.process_name],
        ["Command", ev.command],
        ["Log path", ev.log_path]
      ])}
    `);
    return;
  }

  if (mode === "packet") {
    setPanelContent(id, `
      <div class="detail-title">Full event packet</div>
      ${detailPre(prettyJson(item))}
    `);
  }
}

function toggleAlertPanel(scope, index, mode) {
  const item = alertPools[scope]?.[index];
  if (!item) return;

  const alert = item.alert || {};
  const id = `${scope}-alert-panel-${index}`;
  const el = $(id);

  if (openedPanels.has(id) && el?.dataset.mode === mode) {
    closePanel(id);
    return;
  }

  if (el) el.dataset.mode = mode;

  if (mode === "description") {
    setPanelContent(id, `
      <div class="detail-title">Alert description</div>
      ${detailPre(item.description || alert.description || "No description available")}
    `);
    return;
  }

  if (mode === "source_raw") {
    setPanelContent(id, `
      <div class="detail-title">Source raw log</div>
      ${detailPre(rawAlertText(item))}
      <div class="detail-title second">Alert fields</div>
      ${detailFields([
        ["Alert ID", item.id],
        ["Rule name", item.rule_name || alert.rule_name],
        ["Rule ID", alert.rule_id],
        ["Severity", item.severity || alert.severity],
        ["Title", item.title || alert.title],
        ["Host", alert.host],
        ["Event type", alert.event_type],
        ["Source", alert.source],
        ["Source type", alert.source_type],
        ["Group by", alert.group_by],
        ["Group key", alert.group_key],
        ["Program", alert.program],
        ["User", alert.user],
        ["Source event ID", alert.source_event_id],
        ["Count", alert.count],
        ["Threshold", alert.threshold],
        ["Window seconds", alert.window_seconds],
        ["Suppress seconds", alert.suppress_seconds]
      ])}
    `);
    return;
  }

  if (mode === "packet") {
    setPanelContent(id, `
      <div class="detail-title">Full alert packet</div>
      ${detailPre(prettyJson(item))}
    `);
  }
}

function renderEventCard(item, index, scope = "main") {
  const ev = item.event || {};
  const title = item.event_type || ev.event_type || "unknown_event";
  const panelId = `${scope}-event-panel-${index}`;

  return `
    <article class="event-card">
      <div class="card-main">
        <div>
          <div class="card-kicker">${escapeHtml(item.source_type || item.source || "event")}</div>
          <h4>${escapeHtml(title)}</h4>
        </div>
        <time>${escapeHtml(item.received_at || item.ts || "")}</time>
      </div>

      <div class="badge-row">
        <span class="sev ${severityClass(item.severity)}">${escapeHtml(item.severity || "info")}</span>
        <span class="badge">${escapeHtml(item.host || ev.host || "unknown host")}</span>
        <span class="badge">${escapeHtml(item.source || ev.source || "unknown source")}</span>
        <span class="badge">${escapeHtml(item.event_id || ev.event_id || "no uuid")}</span>
      </div>

      <div class="card-actions">
        <button onclick="toggleEventPanel('${escapeAttr(scope)}', ${index}, 'raw')">Raw</button>
        <button onclick="toggleEventPanel('${escapeAttr(scope)}', ${index}, 'fields')">Fields</button>
        <button onclick="toggleEventPanel('${escapeAttr(scope)}', ${index}, 'packet')">Packet</button>
      </div>

      ${detailBox(panelId)}
    </article>
  `;
}

function renderAlertCard(item, index, scope = "main") {
  const alert = item.alert || {};
  const title = item.title || alert.title || "Alert";
  const ruleName = item.rule_name || alert.rule_name || alert.rule_id || "unknown_rule";
  const panelId = `${scope}-alert-panel-${index}`;

  return `
    <article class="alert-card sev-border-${severityClass(item.severity || alert.severity)}">
      <div class="card-main">
        <div>
          <div class="card-kicker">${escapeHtml(ruleName)}</div>
          <h4>${escapeHtml(title)}</h4>
        </div>
        <time>${escapeHtml(item.ts || alert.ts || "")}</time>
      </div>

      <div class="badge-row">
        <span class="sev ${severityClass(item.severity || alert.severity)}">${escapeHtml(item.severity || alert.severity || "info")}</span>
        <span class="badge">${escapeHtml(alert.host || "unknown host")}</span>
        <span class="badge">${escapeHtml(alert.source_type || "unknown source type")}</span>
        <span class="badge">${escapeHtml(alert.group_key || "no group")}</span>
      </div>

      <div class="card-actions">
        <button onclick="toggleAlertPanel('${escapeAttr(scope)}', ${index}, 'description')">Description</button>
        <button onclick="toggleAlertPanel('${escapeAttr(scope)}', ${index}, 'source_raw')">Source raw</button>
        <button onclick="toggleAlertPanel('${escapeAttr(scope)}', ${index}, 'packet')">Packet</button>
      </div>

      ${detailBox(panelId)}
    </article>
  `;
}

function renderBars(target, data, severityMode = false) {
  const entries = Object.entries(data || {})
    .map(([key, value]) => [key, Number(value) || 0])
    .sort((a, b) => b[1] - a[1]);

  if (!entries.length) {
    target.innerHTML = `<div class="empty">No data</div>`;
    return;
  }

  const max = Math.max(...entries.map(([, count]) => count), 1);

  target.innerHTML = entries.map(([key, count]) => {
    const width = Math.max(4, Math.round((count / max) * 100));
    const cls = severityMode ? `bar-fill sev-${severityClass(key)}` : "bar-fill";

    return `
      <div class="bar-row">
        <div class="bar-top">
          <span>${escapeHtml(key)}</span>
          <strong>${formatNumber(count)}</strong>
        </div>
        <div class="bar-track">
          <div class="${cls}" style="width:${width}%"></div>
        </div>
      </div>
    `;
  }).join("");
}

function renderTimeline(target, points) {
  const rows = Array.isArray(points) ? points : [];

  if (!rows.length) {
    target.innerHTML = `<div class="empty">No timeline data</div>`;
    return;
  }

  const max = Math.max(...rows.map((row) => Number(row.count) || 0), 1);

  target.innerHTML = `
    <div class="timeline-bars">
      ${rows.map((row) => {
        const count = Number(row.count) || 0;
        const height = Math.max(4, Math.round((count / max) * 100));

        return `
          <div class="timeline-column" title="${escapeAttr(row.bucket)} — ${count}">
            <div class="timeline-fill" style="height:${height}%"></div>
            <span>${escapeHtml(String(row.bucket || "").slice(11, 16))}</span>
          </div>
        `;
      }).join("")}
    </div>
  `;
}

function renderDashboard() {
  const high = Number(stats.high_alerts || 0);
  const critical = Number(stats.critical_alerts || 0);
  const hot = high + critical;
  const activeRules = allRules.filter((rule) => rule.enabled !== false).length;

  topTotalEvents.textContent = formatNumber(stats.total_events || 0);
  topHotAlerts.textContent = formatNumber(hot);

  kpiEvents.textContent = formatNumber(stats.total_events || 0);
  kpiAlerts.textContent = formatNumber(stats.total_alerts || 0);
  kpiHotAlerts.textContent = formatNumber(hot);
  kpiRules.textContent = formatNumber(activeRules);

  renderTimeline(eventsTimeline, stats.events_over_time || []);
  renderBars(severityBars, stats.alerts_by_severity || {}, true);
  renderBars(sourceBars, stats.events_by_source_type || {}, false);

  const dashEvents = allEvents.slice(0, 4);
  const dashAlerts = allAlerts.slice(0, 4);

  eventPools["dash-events"] = dashEvents;
  alertPools["dash-alerts"] = dashAlerts;

  dashboardEvents.innerHTML = dashEvents
    .map((item, index) => renderEventCard(item, index, "dash-events"))
    .join("") || `<div class="empty">No events loaded</div>`;

  dashboardAlerts.innerHTML = dashAlerts
    .map((item, index) => renderAlertCard(item, index, "dash-alerts"))
    .join("") || `<div class="empty">No alerts loaded</div>`;
}

function parseKql(query) {
  const result = {
    terms: [],
    fields: {}
  };

  const re = /(\w+):("[^"]+"|\S+)|"([^"]+)"|(\S+)/g;
  let match;

  while ((match = re.exec(query)) !== null) {
    if (match[1]) {
      const key = lower(match[1]);
      let value = match[2] || "";
      value = value.replace(/^"|"$/g, "");
      result.fields[key] = lower(value);
    } else {
      const value = match[3] || match[4] || "";
      if (value) result.terms.push(lower(value));
    }
  }

  return result;
}

function fieldMatches(value, expected) {
  if (!expected) return true;
  return lower(value).includes(expected);
}

function filteredEvents() {
  const query = eventSearchInput.value.trim();
  const parsed = parseKql(query);

  return allEvents.filter((item) => {
    const ev = item.event || {};

    const fieldMap = {
      id: item.id,
      event_id: item.event_id || ev.event_id,
      uuid: item.event_id || ev.event_id,
      ts: item.ts || ev.ts,
      received_at: item.received_at || ev.received_at,
      host: item.host || ev.host,
      event_type: item.event_type || ev.event_type,
      type: item.event_type || ev.event_type,
      source: item.source || ev.source,
      source_type: item.source_type || ev.source_type,
      severity: item.severity || ev.severity,
      user: ev.user,
      src_ip: ev.src_ip,
      ip: ev.src_ip,
      program: ev.program || ev.process || ev.process_name,
      process: ev.process || ev.process_name || ev.program,
      command: ev.command,
      raw: rawEventText(item),
      message: ev.message
    };

    for (const [key, expected] of Object.entries(parsed.fields)) {
      const value = fieldMap[key] ?? objectText(ev);
      if (!fieldMatches(value, expected)) return false;
    }

    const fullText = lower([
      objectText(item),
      rawEventText(item)
    ].join(" "));

    for (const term of parsed.terms) {
      if (!fullText.includes(term)) return false;
    }

    return true;
  });
}

function filteredAlerts() {
  const query = alertSearchInput.value.trim();
  const parsed = parseKql(query);

  return allAlerts.filter((item) => {
    const alert = item.alert || {};

    const fieldMap = {
      id: item.id,
      ts: item.ts || alert.ts,
      rule: item.rule_name || alert.rule_name || alert.rule_id,
      rule_name: item.rule_name || alert.rule_name,
      rule_id: alert.rule_id,
      severity: item.severity || alert.severity,
      title: item.title || alert.title,
      description: item.description || alert.description,
      host: alert.host,
      event_type: alert.event_type,
      type: alert.event_type,
      source: alert.source,
      source_type: alert.source_type,
      group: alert.group_key,
      group_key: alert.group_key,
      group_by: alert.group_by,
      user: alert.user,
      program: alert.program,
      raw: rawAlertText(item)
    };

    for (const [key, expected] of Object.entries(parsed.fields)) {
      const value = fieldMap[key] ?? objectText(alert);
      if (!fieldMatches(value, expected)) return false;
    }

    const fullText = lower([
      objectText(item),
      rawAlertText(item)
    ].join(" "));

    for (const term of parsed.terms) {
      if (!fullText.includes(term)) return false;
    }

    return true;
  });
}

function renderEvents() {
  const items = filteredEvents();
  eventsCount.textContent = String(items.length);

  eventPools["events"] = items;

  if (!items.length) {
    eventsList.innerHTML = `<div class="empty">No events match current query</div>`;
    return;
  }

  eventsList.innerHTML = items
    .map((item, index) => renderEventCard(item, index, "events"))
    .join("");
}

function renderAlerts() {
  const items = filteredAlerts();
  alertsCount.textContent = String(items.length);

  alertPools["alerts"] = items;

  if (!items.length) {
    alertsList.innerHTML = `<div class="empty">No alerts match current query</div>`;
    return;
  }

  alertsList.innerHTML = items
    .map((item, index) => renderAlertCard(item, index, "alerts"))
    .join("");
}

function renderConditions() {
  if (!currentConditions.length) {
    conditionsList.innerHTML = `<div class="empty small">No conditions added</div>`;
    return;
  }

  conditionsList.innerHTML = currentConditions.map((condition, index) => {
    const valueText = Array.isArray(condition.values)
      ? condition.values.join(", ")
      : condition.value || "";

    return `
      <div class="condition-pill">
        <span>
          <strong>${escapeHtml(condition.field)}</strong>
          ${escapeHtml(condition.op)}
          <em>${escapeHtml(valueText || "—")}</em>
        </span>
        <button onclick="removeCondition(${index})">Remove</button>
      </div>
    `;
  }).join("");
}

function toggleRuleDetails(index) {
  const rule = allRules[index];
  if (!rule) return;

  const id = `rule-detail-${index}`;
  const el = $(id);

  if (openedPanels.has(id)) {
    closePanel(id);
    return;
  }

  if (el) el.dataset.mode = "rule";

  const conditions = Array.isArray(rule.conditions) ? rule.conditions : [];

  setPanelContent(id, `
    <div class="detail-title">Rule details</div>
    ${detailFields([
      ["ID", rule.id],
      ["Enabled", rule.enabled !== false ? "yes" : "no"],
      ["Type", rule.type],
      ["Severity", rule.severity],
      ["Event types", joinCsv(rule.event_types || [])],
      ["Source types", joinCsv(rule.source_types || [])],
      ["Group by", rule.group_by],
      ["Threshold", rule.threshold],
      ["Window", rule.window_sec ? `${rule.window_sec}s` : "—"],
      ["Suppress", rule.suppress_sec ? `${rule.suppress_sec}s` : "0s"],
      ["Conditions", conditions.length]
    ])}

    <div class="detail-title second">Description</div>
    ${detailPre(rule.description || "No description")}

    <div class="detail-title second">Full rule packet</div>
    ${detailPre(prettyJson(rule))}
  `);
}

function renderRules() {
  rulesCount.textContent = String(allRules.length);

  if (!allRules.length) {
    rulesList.innerHTML = `<div class="empty">No rules found</div>`;
    return;
  }

  rulesList.innerHTML = allRules.map((rule, index) => {
    const enabled = rule.enabled !== false;
    const conditionsCount = Array.isArray(rule.conditions) ? rule.conditions.length : 0;
    const eventTypes = Array.isArray(rule.event_types) && rule.event_types.length
      ? rule.event_types.join(", ")
      : "any";
    const sourceTypes = Array.isArray(rule.source_types) && rule.source_types.length
      ? rule.source_types.join(", ")
      : "any";

    return `
      <article class="rule-card ${enabled ? "" : "disabled"}">
        <div class="rule-main-row">
          <div class="rule-main-left">
            <div class="rule-id">${escapeHtml(rule.id || "unknown_rule")}</div>
            <div class="rule-title-text">${escapeHtml(rule.title || "Detection rule")}</div>
          </div>

          <div class="rule-main-right">
            <span class="sev ${severityClass(rule.severity)}">${escapeHtml(rule.severity || "medium")}</span>
            <span class="rule-state ${enabled ? "enabled" : "disabled"}">${enabled ? "enabled" : "disabled"}</span>
          </div>
        </div>

        <div class="rule-mini-grid">
          <div>
            <span>Type</span>
            <strong>${escapeHtml(rule.type || "threshold")}</strong>
          </div>
          <div>
            <span>Event types</span>
            <strong>${escapeHtml(eventTypes)}</strong>
          </div>
          <div>
            <span>Source types</span>
            <strong>${escapeHtml(sourceTypes)}</strong>
          </div>
          <div>
            <span>Group by</span>
            <strong>${escapeHtml(rule.group_by || "—")}</strong>
          </div>
          <div>
            <span>Threshold</span>
            <strong>${escapeHtml(rule.threshold ?? "—")}</strong>
          </div>
          <div>
            <span>Window</span>
            <strong>${escapeHtml(rule.window_sec ? `${rule.window_sec}s` : "—")}</strong>
          </div>
          <div>
            <span>Conditions</span>
            <strong>${conditionsCount}</strong>
          </div>
        </div>

        <div class="rule-actions">
          <button onclick="toggleRuleDetails(${index})">Details</button>
          <button onclick="editRule('${escapeAttr(rule.id)}')">Edit</button>
          <button onclick="toggleRule('${escapeAttr(rule.id)}')">${enabled ? "Disable" : "Enable"}</button>
          <button class="delete" onclick="deleteRule('${escapeAttr(rule.id)}')">Delete</button>
        </div>

        ${detailBox(`rule-detail-${index}`)}
      </article>
    `;
  }).join("");
}

function renderAll() {
  closeAllPanels();
  renderDashboard();
  renderEvents();
  renderAlerts();
  renderRules();
}

async function loadAll(options = {}) {
  const silent = options.silent === true;

  try {
    if (!silent) setStatus("Loading...");

    const limit = getLimit();

    const [statsData, eventsData, alertsData, rulesData] = await Promise.all([
      fetchJson("/api/stats"),
      fetchJson(`/api/events?limit=${limit}`),
      fetchJson(`/api/alerts?limit=${limit}`),
      fetchJson("/api/rules")
    ]);

    stats = statsData || {};
    allEvents = Array.isArray(eventsData.events) ? eventsData.events : [];
    allAlerts = Array.isArray(alertsData.alerts) ? alertsData.alerts : [];
    allRules = Array.isArray(rulesData.rules) ? rulesData.rules : [];

    renderAll();

    setStatus(autoRefreshEnabled ? "Auto-refresh ON" : "Manual mode");
    touchUpdateTime();
  } catch (err) {
    console.error(err);
    setStatus("Load failed");
    if (!silent) alert(`Failed to load dashboard data: ${err.message}`);
  }
}

function activateTab(tab) {
  document.querySelectorAll(".nav-btn").forEach((btn) => {
    btn.classList.toggle("active", btn.dataset.tab === tab);
  });

  document.querySelectorAll(".tab-view").forEach((view) => {
    view.classList.remove("active");
  });

  const view = $(`tab${tab.charAt(0).toUpperCase()}${tab.slice(1)}`);
  if (view) view.classList.add("active");

  pageTitle.textContent = tabMeta[tab]?.title || "Mini SIEM";
  pageSubtitle.textContent = tabMeta[tab]?.subtitle || "";
}

function clearEventFilters() {
  eventSearchInput.value = "";
  renderEvents();
}

function clearAlertFilters() {
  alertSearchInput.value = "";
  renderAlerts();
}

function setRuleMessage(message, type = "info") {
  ruleMessage.textContent = message || "";
  ruleMessage.className = `rule-message ${type}`;
}

function ruleFromForm() {
  const id = ruleIdInput.value.trim();

  if (!id) {
    throw new Error("Rule ID is required");
  }

  return {
    id,
    enabled: ruleEnabledInput.checked,
    type: ruleTypeInput.value || "threshold",
    event_types: splitCsv(ruleEventTypesInput.value),
    source_types: splitCsv(ruleSourceTypesInput.value),
    group_by: ruleGroupByInput.value.trim(),
    threshold: Math.max(1, Number(ruleThresholdInput.value) || 1),
    window_sec: Math.max(1, Number(ruleWindowInput.value) || 60),
    suppress_sec: Math.max(0, Number(ruleSuppressInput.value) || 0),
    severity: ruleSeverityInput.value || "medium",
    title: ruleTitleInput.value.trim() || id,
    description: ruleDescriptionInput.value.trim(),
    conditions: currentConditions
  };
}

function fillRuleForm(rule) {
  editingRuleId = rule.id || "";

  ruleFormTitle.textContent = `Edit rule: ${editingRuleId}`;
  ruleIdInput.disabled = true;
  ruleIdInput.value = rule.id || "";

  ruleEnabledInput.checked = rule.enabled !== false;
  ruleTypeInput.value = rule.type || "threshold";
  ruleSeverityInput.value = rule.severity || "medium";
  ruleEventTypesInput.value = joinCsv(rule.event_types || []);
  ruleSourceTypesInput.value = joinCsv(rule.source_types || []);
  ruleGroupByInput.value = rule.group_by || "";
  ruleThresholdInput.value = rule.threshold ?? 1;
  ruleWindowInput.value = rule.window_sec ?? 60;
  ruleSuppressInput.value = rule.suppress_sec ?? 0;
  ruleTitleInput.value = rule.title || "";
  ruleDescriptionInput.value = rule.description || "";

  currentConditions = Array.isArray(rule.conditions)
    ? JSON.parse(JSON.stringify(rule.conditions))
    : [];

  renderConditions();

  createRuleBtn.disabled = true;
  updateRuleBtn.disabled = false;

  setRuleMessage(`Editing ${editingRuleId}`, "info");
}

function clearRuleForm() {
  editingRuleId = "";

  ruleFormTitle.textContent = "Create rule";
  ruleIdInput.disabled = false;
  ruleIdInput.value = "";

  ruleEnabledInput.checked = true;
  ruleTypeInput.value = "threshold";
  ruleSeverityInput.value = "medium";
  ruleEventTypesInput.value = "";
  ruleSourceTypesInput.value = "";
  ruleGroupByInput.value = "";
  ruleThresholdInput.value = "5";
  ruleWindowInput.value = "300";
  ruleSuppressInput.value = "60";
  ruleTitleInput.value = "";
  ruleDescriptionInput.value = "";

  conditionFieldInput.value = "";
  conditionOpInput.value = "equals";
  conditionValueInput.value = "";
  conditionValuesInput.value = "";

  currentConditions = [];
  renderConditions();

  createRuleBtn.disabled = false;
  updateRuleBtn.disabled = true;

  setRuleMessage("", "info");
}

function addCondition() {
  const field = conditionFieldInput.value.trim();
  const op = conditionOpInput.value.trim();
  const value = conditionValueInput.value.trim();
  const values = splitCsv(conditionValuesInput.value);

  if (!field) {
    setRuleMessage("Condition field is required", "error");
    return;
  }

  const condition = { field, op };

  if (op === "in" || op === "contains_any") {
    condition.values = values.length ? values : splitCsv(value);
  } else if (op !== "exists") {
    condition.value = value;
  }

  currentConditions.push(condition);

  conditionFieldInput.value = "";
  conditionOpInput.value = "equals";
  conditionValueInput.value = "";
  conditionValuesInput.value = "";

  renderConditions();
  setRuleMessage("Condition added", "ok");
}

function removeCondition(index) {
  currentConditions.splice(index, 1);
  renderConditions();
}

function editRule(id) {
  const rule = allRules.find((item) => item.id === id);

  if (!rule) {
    setRuleMessage(`Rule not found: ${id}`, "error");
    return;
  }

  activateTab("rules");
  fillRuleForm(rule);
}

async function createRule() {
  try {
    const rule = ruleFromForm();

    await fetchJson("/api/rules", {
      method: "POST",
      headers: {
        "Content-Type": "application/json"
      },
      body: JSON.stringify(rule)
    });

    setRuleMessage(`Created ${rule.id}`, "ok");
    clearRuleForm();
    await loadAll();
    activateTab("rules");
  } catch (err) {
    setRuleMessage(`Create failed: ${err.message}`, "error");
  }
}

async function updateRule() {
  try {
    if (!editingRuleId) {
      throw new Error("No rule selected");
    }

    const rule = ruleFromForm();
    rule.id = editingRuleId;

    await fetchJson(`/api/rules/${encodeURIComponent(editingRuleId)}`, {
      method: "PUT",
      headers: {
        "Content-Type": "application/json"
      },
      body: JSON.stringify(rule)
    });

    setRuleMessage(`Updated ${editingRuleId}`, "ok");
    clearRuleForm();
    await loadAll();
    activateTab("rules");
  } catch (err) {
    setRuleMessage(`Update failed: ${err.message}`, "error");
  }
}

async function toggleRule(id) {
  try {
    const rule = allRules.find((item) => item.id === id);

    if (!rule) {
      throw new Error(`Rule not found: ${id}`);
    }

    const updated = JSON.parse(JSON.stringify(rule));
    updated.enabled = updated.enabled === false;

    await fetchJson(`/api/rules/${encodeURIComponent(id)}`, {
      method: "PUT",
      headers: {
        "Content-Type": "application/json"
      },
      body: JSON.stringify(updated)
    });

    setRuleMessage(`${id} ${updated.enabled ? "enabled" : "disabled"}`, "ok");
    await loadAll();
    activateTab("rules");
  } catch (err) {
    setRuleMessage(`Toggle failed: ${err.message}`, "error");
  }
}

async function deleteRule(id) {
  if (!confirm(`Delete rule ${id}?`)) return;

  try {
    await fetchJson(`/api/rules/${encodeURIComponent(id)}`, {
      method: "DELETE"
    });

    setRuleMessage(`Deleted ${id}`, "ok");

    if (editingRuleId === id) {
      clearRuleForm();
    }

    await loadAll();
    activateTab("rules");
  } catch (err) {
    setRuleMessage(`Delete failed: ${err.message}`, "error");
  }
}

function applyTheme() {
  document.body.classList.toggle("theme-cute", currentTheme === "cute");

  themeToggleBtn.textContent = currentTheme === "cute"
    ? "Theme: Cute pink"
    : "Theme: Horror red";

  localStorage.setItem("mini_siem_theme", currentTheme);
}

function toggleTheme() {
  currentTheme = currentTheme === "horror" ? "cute" : "horror";
  applyTheme();
}

function updateAutoRefreshUi() {
  autoRefreshBtn.textContent = autoRefreshEnabled
    ? "Auto-refresh: ON"
    : "Auto-refresh: OFF";

  runtimeNote.textContent = autoRefreshEnabled
    ? `Auto-refresh every ${AUTO_REFRESH_MS / 1000}s.`
    : "Auto-refresh is disabled.";

  setStatus(autoRefreshEnabled ? "Auto-refresh ON" : "Manual mode");
}

function stopAutoRefresh() {
  if (autoRefreshTimer) {
    clearInterval(autoRefreshTimer);
    autoRefreshTimer = null;
  }
}

function startAutoRefresh() {
  stopAutoRefresh();

  autoRefreshTimer = setInterval(() => {
    loadAll({ silent: true });
  }, AUTO_REFRESH_MS);
}

function toggleAutoRefresh() {
  autoRefreshEnabled = !autoRefreshEnabled;

  if (autoRefreshEnabled) {
    startAutoRefresh();
  } else {
    stopAutoRefresh();
  }

  updateAutoRefreshUi();
}

function bindEvents() {
  document.querySelectorAll(".nav-btn").forEach((btn) => {
    btn.addEventListener("click", () => {
      activateTab(btn.dataset.tab);
    });
  });

  refreshBtn.addEventListener("click", () => loadAll());
  autoRefreshBtn.addEventListener("click", toggleAutoRefresh);
  themeToggleBtn.addEventListener("click", toggleTheme);
  clearOpenBtn.addEventListener("click", closeAllPanels);
  limitInput.addEventListener("change", () => loadAll());

  eventSearchInput.addEventListener("input", renderEvents);
  clearEventFiltersBtn.addEventListener("click", clearEventFilters);

  alertSearchInput.addEventListener("input", renderAlerts);
  clearAlertFiltersBtn.addEventListener("click", clearAlertFilters);

  addConditionBtn.addEventListener("click", addCondition);
  createRuleBtn.addEventListener("click", createRule);
  updateRuleBtn.addEventListener("click", updateRule);
  clearRuleFormBtn.addEventListener("click", clearRuleForm);
}

window.toggleEventPanel = toggleEventPanel;
window.toggleAlertPanel = toggleAlertPanel;
window.toggleRuleDetails = toggleRuleDetails;
window.editRule = editRule;
window.toggleRule = toggleRule;
window.deleteRule = deleteRule;
window.removeCondition = removeCondition;

applyTheme();
updateAutoRefreshUi();
bindEvents();
renderConditions();
activateTab("dashboard");
loadAll();
