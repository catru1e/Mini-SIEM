const eventsList = document.getElementById("eventsList");
const alertsList = document.getElementById("alertsList");
const rulesList = document.getElementById("rulesList");

const eventsCount = document.getElementById("eventsCount");
const alertsCount = document.getElementById("alertsCount");
const rulesCount = document.getElementById("rulesCount");
const statusText = document.getElementById("statusText");

const statEvents = document.getElementById("statEvents");
const statAlerts = document.getElementById("statAlerts");
const statHighAlerts = document.getElementById("statHighAlerts");
const statRules = document.getElementById("statRules");
const statLastUpdate = document.getElementById("statLastUpdate");

const eventTypeFilter = document.getElementById("eventTypeFilter");
const severityFilter = document.getElementById("severityFilter");
const limitInput = document.getElementById("limitInput");
const refreshBtn = document.getElementById("refreshBtn");
const reloadRulesBtn = document.getElementById("reloadRulesBtn");
const loadRulesBtn = document.getElementById("loadRulesBtn");

const ruleMessage = document.getElementById("ruleMessage");
const ruleFormTitle = document.getElementById("ruleFormTitle");
const ruleIdInput = document.getElementById("ruleIdInput");
const ruleEnabledInput = document.getElementById("ruleEnabledInput");
const ruleTypeInput = document.getElementById("ruleTypeInput");
const ruleSeverityInput = document.getElementById("ruleSeverityInput");
const ruleEventTypesInput = document.getElementById("ruleEventTypesInput");
const ruleSourceTypesInput = document.getElementById("ruleSourceTypesInput");
const ruleGroupByInput = document.getElementById("ruleGroupByInput");
const ruleThresholdInput = document.getElementById("ruleThresholdInput");
const ruleWindowInput = document.getElementById("ruleWindowInput");
const ruleSuppressInput = document.getElementById("ruleSuppressInput");
const ruleTitleInput = document.getElementById("ruleTitleInput");
const ruleDescriptionInput = document.getElementById("ruleDescriptionInput");

const conditionFieldInput = document.getElementById("conditionFieldInput");
const conditionOpInput = document.getElementById("conditionOpInput");
const conditionValueInput = document.getElementById("conditionValueInput");
const conditionValuesInput = document.getElementById("conditionValuesInput");
const addConditionBtn = document.getElementById("addConditionBtn");
const conditionsList = document.getElementById("conditionsList");

const createRuleBtn = document.getElementById("createRuleBtn");
const updateRuleBtn = document.getElementById("updateRuleBtn");
const clearRuleFormBtn = document.getElementById("clearRuleFormBtn");

//fix web update-a
const openedJson = new Set();

let allEvents = [];
let allAlerts = [];
let allRules = [];
let currentConditions = [];
let editingRuleId = "";
let eventSource = null;

function escapeHtml(value) {
  return String(value ?? "")
    .replaceAll("&", "&amp;")
    .replaceAll("<", "&lt;")
    .replaceAll(">", "&gt;")
    .replaceAll('"', "&quot;")
    .replaceAll("'", "&#039;");
}

function severityClass(severity) {
  const s = (severity || "info").toLowerCase();
  return ["info", "low", "medium", "high"].includes(s) ? s : "info";
}

function splitCsv(value) {
  return String(value || "")
    .split(",")
    .map(v => v.trim())
    .filter(Boolean);
}

function joinCsv(values) {
  if (!Array.isArray(values)) return "";
  return values.join(", ");
}

function shortEventDescription(item) {
  if (item.event?.description) return item.event.description;
  if (item.event?.message) return item.event.message;

  if (item.event_type === "privilege_escalation") {
    const user = item.event?.user || "unknown";
    const command = item.event?.command || "unknown command";
    return `sudo command by ${user}: ${command}`;
  }

  if (item.event?.src_ip) return `Source IP: ${item.event.src_ip}`;
  if (item.event?.raw) return item.event.raw;

  return "No additional description";
}

function eventCommand(item) {
  if (item.event?.command) return item.event.command;
  return "—";
}

function updateStats(events, alerts) {
  statEvents.textContent = String(events.length);
  statAlerts.textContent = String(alerts.length);
  statHighAlerts.textContent = String(
    alerts.filter(a => (a.severity || "").toLowerCase() === "high").length
  );
  statRules.textContent = String(allRules.length);
  statLastUpdate.textContent = new Date().toLocaleTimeString();
}

function createMetaItem(label, value) {
  return `
    <div class="meta-item">
      <div class="meta-label">${escapeHtml(label)}</div>
      <div class="meta-value">${escapeHtml(value || "—")}</div>
    </div>
  `;
}

function renderEvents(items) {
  eventsCount.textContent = String(items.length);

  if (!items.length) {
    eventsList.innerHTML = `<div class="empty">No events found for current filters</div>`;
    return;
  }

  eventsList.innerHTML = items.map(item => {
    const rawId = `event-json-${item.id ?? "live"}-${item.ts ?? ""}-${item.event_type ?? ""}`;
    return `
      <div class="item-card">
        <div class="item-top">
          <div>
            <div class="item-title">${escapeHtml(item.event_type || "Unknown event")}</div>
          </div>
          <div class="item-time">${escapeHtml(item.ts || "")}</div>
        </div>

        <div class="meta-grid">
          ${createMetaItem("Host", item.host || "unknown")}
          ${createMetaItem("Source", item.source || "unknown")}
          ${createMetaItem("Source type", item.source_type || "—")}
          ${createMetaItem("Severity", item.severity || "info")}
          ${createMetaItem("DB ID", item.id ?? "live")}
          ${createMetaItem("Event UUID", item.event_id || item.event?.event_id || "—")}
          ${createMetaItem("Received", item.received_at || item.event?.received_at || "—")}
	  ${createMetaItem("User", item.event?.user || "—")}
  	  ${createMetaItem("Command", eventCommand(item))}
        </div>

        <div>
          <span class="severity ${severityClass(item.severity)}">${escapeHtml(item.severity || "info")}</span>
        </div>

        <div class="description">${escapeHtml(shortEventDescription(item))}</div>

        <button class="json-toggle" onclick="toggleJson('${rawId}')">Show raw JSON</button>
        <pre id="${rawId}" class="raw-json ${openedJson.has(rawId) ? 'open' : ''}">${escapeHtml(JSON.stringify(item.event || {}, null, 2))}</pre>
      </div>
    `;
  }).join("");
}

function renderAlerts(items) {
  alertsCount.textContent = String(items.length);

  if (!items.length) {
    alertsList.innerHTML = `<div class="empty">No alerts found for current filters</div>`;
    return;
  }

  alertsList.innerHTML = items.map(item => {
    const rawId = `alert-json-${item.id ?? "live"}-${item.ts ?? ""}-${item.rule_name ?? ""}`;
    return `
      <div class="item-card">
        <div class="item-top">
          <div>
            <div class="item-title">${escapeHtml(item.title || "Alert")}</div>
          </div>
          <div class="item-time">${escapeHtml(item.ts || "")}</div>
        </div>

        <div class="meta-grid">
          ${createMetaItem("Rule", item.rule_name || "—")}
          ${createMetaItem("Severity", item.severity || "—")}
          ${createMetaItem("Alert ID", item.id ?? "live")}
          ${createMetaItem("Type", item.alert?.rule_id ? "Rule engine" : "Detection/Correlation")}
          ${createMetaItem("Group", item.alert?.group_key || "—")}
          ${createMetaItem("Source type", item.alert?.source_type || "—")}
        </div>

        <div>
          <span class="severity ${severityClass(item.severity)}">${escapeHtml(item.severity || "info")}</span>
        </div>

        <div class="description">${escapeHtml(item.description || "No description")}</div>

        <button class="json-toggle" onclick="toggleJson('${rawId}')">Show raw JSON</button>
        <pre id="${rawId}" class="raw-json ${openedJson.has(rawId) ? 'open' : ''}">${escapeHtml(JSON.stringify(item.alert || {}, null, 2))}</pre>
      </div>
    `;
  }).join("");
}

function renderConditions() {
  if (!currentConditions.length) {
    conditionsList.innerHTML = `<div class="empty small-empty">No conditions added</div>`;
    return;
  }

  conditionsList.innerHTML = currentConditions.map((condition, index) => {
    const valuesText = Array.isArray(condition.values) && condition.values.length
      ? condition.values.join(", ")
      : (condition.value || "");

    return `
      <div class="condition-pill">
        <div>
          <strong>${escapeHtml(condition.field)}</strong>
          <span>${escapeHtml(condition.op)}</span>
          <em>${escapeHtml(valuesText || "—")}</em>
        </div>
        <button onclick="removeCondition(${index})">Remove</button>
      </div>
    `;
  }).join("");
}

function renderRules() {
  rulesCount.textContent = String(allRules.length);
  statRules.textContent = String(allRules.length);

  if (!allRules.length) {
    rulesList.innerHTML = `<div class="empty">No rules found</div>`;
    return;
  }

  rulesList.innerHTML = allRules.map(rule => {
    const enabled = rule.enabled !== false;
    const eventTypes = Array.isArray(rule.event_types) ? rule.event_types.join(", ") : (rule.event_type || "any");
    const sourceTypes = Array.isArray(rule.source_types) ? rule.source_types.join(", ") : (rule.source_type || "any");
    const conditionsCount = Array.isArray(rule.conditions) ? rule.conditions.length : 0;

    return `
      <div class="rule-card ${enabled ? "" : "disabled-rule"}">
        <div class="rule-card-top">
          <div>
            <div class="rule-title">${escapeHtml(rule.id || "unknown_rule")}</div>
            <div class="rule-subtitle">${escapeHtml(rule.title || "Detection rule")}</div>
          </div>
          <span class="rule-state ${enabled ? "enabled" : "disabled"}">${enabled ? "enabled" : "disabled"}</span>
        </div>

        <div class="meta-grid">
          ${createMetaItem("Type", rule.type || "threshold")}
          ${createMetaItem("Severity", rule.severity || "medium")}
          ${createMetaItem("Event types", eventTypes)}
          ${createMetaItem("Source types", sourceTypes)}
          ${createMetaItem("Group by", rule.group_by || "—")}
          ${createMetaItem("Threshold", rule.threshold ?? "—")}
          ${createMetaItem("Window", rule.window_sec ? `${rule.window_sec}s` : "—")}
          ${createMetaItem("Suppress", rule.suppress_sec ? `${rule.suppress_sec}s` : "0s")}
          ${createMetaItem("Conditions", conditionsCount)}
        </div>

        <div class="description">${escapeHtml(rule.description || "No description")}</div>

        <div class="rule-actions">
          <button class="secondary-btn" onclick="editRule('${escapeHtml(rule.id)}')">Edit</button>
          <button class="secondary-btn" onclick="toggleRule('${escapeHtml(rule.id)}')">${enabled ? "Disable" : "Enable"}</button>
          <button class="danger-btn" onclick="deleteRule('${escapeHtml(rule.id)}')">Delete</button>
        </div>
      </div>
    `;
  }).join("");
}

function populateFilters(events, alerts) {
  const eventTypes = new Set();
  const severities = new Set();

  for (const item of events) {
    if (item.event_type) eventTypes.add(item.event_type);
    if (item.severity) severities.add(item.severity);
  }

  for (const item of alerts) {
    if (item.severity) severities.add(item.severity);
  }

  const currentEventType = eventTypeFilter.value;
  const currentSeverity = severityFilter.value;

  eventTypeFilter.innerHTML =
    `<option value="">All</option>` +
    [...eventTypes].sort().map(v => `<option value="${escapeHtml(v)}">${escapeHtml(v)}</option>`).join("");

  severityFilter.innerHTML =
    `<option value="">All</option>` +
    [...severities].sort().map(v => `<option value="${escapeHtml(v)}">${escapeHtml(v)}</option>`).join("");

  eventTypeFilter.value = [...eventTypes].includes(currentEventType) ? currentEventType : "";
  severityFilter.value = [...severities].includes(currentSeverity) ? currentSeverity : "";
}

function applyFilters() {
  const selectedEventType = eventTypeFilter.value;
  const selectedSeverity = severityFilter.value;

  const filteredEvents = allEvents.filter(item => {
    if (selectedEventType && item.event_type !== selectedEventType) return false;
    if (selectedSeverity && item.severity !== selectedSeverity) return false;
    return true;
  });

  const filteredAlerts = allAlerts.filter(item => {
    if (selectedSeverity && item.severity !== selectedSeverity) return false;
    return true;
  });

  renderEvents(filteredEvents);
  renderAlerts(filteredAlerts);
  updateStats(filteredEvents, filteredAlerts);
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
    const message = data.message || `HTTP ${response.status} for ${url}`;
    throw new Error(message);
  }

  return data;
}

function trimToLimit(items, limit) {
  const safeLimit = Math.max(1, Math.min(500, Number(limit) || 100));
  return items.slice(0, safeLimit);
}

function addLiveEvent(item) {
  allEvents = [item, ...allEvents];
  allEvents = trimToLimit(allEvents, limitInput.value);
  populateFilters(allEvents, allAlerts);
  applyFilters();
}

function addLiveAlert(item) {
  allAlerts = [item, ...allAlerts];
  allAlerts = trimToLimit(allAlerts, limitInput.value);
  populateFilters(allEvents, allAlerts);
  applyFilters();
}

async function loadData() {
  try {
    statusText.textContent = "Updating...";

    const limit = Number(limitInput.value) || 100;
    const safeLimit = Math.max(1, Math.min(500, limit));

    const [eventsData, alertsData] = await Promise.all([
      fetchJson(`/api/events?limit=${safeLimit}`),
      fetchJson(`/api/alerts?limit=${safeLimit}`)
    ]);

    allEvents = Array.isArray(eventsData.events) ? eventsData.events : [];
    allAlerts = Array.isArray(alertsData.alerts) ? alertsData.alerts : [];

    populateFilters(allEvents, allAlerts);
    applyFilters();

    statusText.textContent = "Online";
    statLastUpdate.textContent = new Date().toLocaleTimeString();
  } catch (err) {
    console.error(err);
    statusText.textContent = "Update failed";
    eventsList.innerHTML = `<div class="error">Failed to load events</div>`;
    alertsList.innerHTML = `<div class="error">Failed to load alerts</div>`;
  }
}

async function loadRules() {
  try {
    setRuleMessage("Loading rules...", "info");

    const data = await fetchJson("/api/rules");
    allRules = Array.isArray(data.rules) ? data.rules : [];

    renderRules();
    setRuleMessage(`Loaded ${allRules.length} rules`, "ok");
  } catch (err) {
    console.error(err);
    setRuleMessage(`Failed to load rules: ${err.message}`, "error");
    rulesList.innerHTML = `<div class="error">Failed to load rules. Check that /api/rules exists.</div>`;
  }
}

function setRuleMessage(message, type = "info") {
  if (!ruleMessage) return;
  ruleMessage.textContent = message || "";
  ruleMessage.className = `rule-message ${type}`;
}

function ruleFromForm() {
  const id = ruleIdInput.value.trim();

  if (!id) {
    throw new Error("Rule ID is required");
  }

  const eventTypes = splitCsv(ruleEventTypesInput.value);
  const sourceTypes = splitCsv(ruleSourceTypesInput.value);

  const rule = {
    id,
    enabled: ruleEnabledInput.checked,
    type: ruleTypeInput.value || "threshold",
    event_types: eventTypes,
    source_types: sourceTypes,
    group_by: ruleGroupByInput.value.trim(),
    suppress_sec: Number(ruleSuppressInput.value) || 0,
    severity: ruleSeverityInput.value || "medium",
    title: ruleTitleInput.value.trim() || id,
    description: ruleDescriptionInput.value.trim(),
    conditions: currentConditions
  };

  if (rule.type === "threshold") {
    rule.threshold = Math.max(1, Number(ruleThresholdInput.value) || 1);
    rule.window_sec = Math.max(1, Number(ruleWindowInput.value) || 60);
  } else {
    rule.threshold = Math.max(1, Number(ruleThresholdInput.value) || 1);
    rule.window_sec = Math.max(1, Number(ruleWindowInput.value) || 60);
  }

  return rule;
}

function fillFormFromRule(rule) {
  editingRuleId = rule.id || "";

  ruleFormTitle.textContent = `Edit rule: ${editingRuleId}`;
  ruleIdInput.value = rule.id || "";
  ruleIdInput.disabled = true;

  ruleEnabledInput.checked = rule.enabled !== false;
  ruleTypeInput.value = rule.type || "threshold";
  ruleSeverityInput.value = rule.severity || "medium";
  ruleEventTypesInput.value = joinCsv(rule.event_types || (rule.event_type ? [rule.event_type] : []));
  ruleSourceTypesInput.value = joinCsv(rule.source_types || (rule.source_type ? [rule.source_type] : []));
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

async function createRule() {
  try {
    const rule = ruleFromForm();

    const data = await fetchJson("/api/rules", {
      method: "POST",
      headers: {
        "Content-Type": "application/json"
      },
      body: JSON.stringify(rule)
    });

    setRuleMessage(`Created rule ${data.id || rule.id}`, "ok");
    clearRuleForm();
    await loadRules();
  } catch (err) {
    console.error(err);
    setRuleMessage(`Create failed: ${err.message}`, "error");
  }
}

async function updateRule() {
  try {
    if (!editingRuleId) {
      throw new Error("No rule selected for editing");
    }

    const rule = ruleFromForm();
    rule.id = editingRuleId;

    const data = await fetchJson(`/api/rules/${encodeURIComponent(editingRuleId)}`, {
      method: "PUT",
      headers: {
        "Content-Type": "application/json"
      },
      body: JSON.stringify(rule)
    });

    setRuleMessage(`Updated rule ${data.id || editingRuleId}`, "ok");
    clearRuleForm();
    await loadRules();
  } catch (err) {
    console.error(err);
    setRuleMessage(`Update failed: ${err.message}`, "error");
  }
}

function editRule(id) {
  const rule = allRules.find(r => r.id === id);
  if (!rule) {
    setRuleMessage(`Rule not found: ${id}`, "error");
    return;
  }

  fillFormFromRule(rule);

  document.querySelector(".rules-panel")?.scrollIntoView({
    behavior: "smooth",
    block: "start"
  });
}

async function toggleRule(id) {
  try {
    const rule = allRules.find(r => r.id === id);
    if (!rule) {
      throw new Error(`Rule not found: ${id}`);
    }

    const updated = JSON.parse(JSON.stringify(rule));
    updated.enabled = updated.enabled === false;

    const data = await fetchJson(`/api/rules/${encodeURIComponent(id)}`, {
      method: "PUT",
      headers: {
        "Content-Type": "application/json"
      },
      body: JSON.stringify(updated)
    });

    setRuleMessage(`Rule ${data.id || id} ${updated.enabled ? "enabled" : "disabled"}`, "ok");
    await loadRules();
  } catch (err) {
    console.error(err);
    setRuleMessage(`Toggle failed: ${err.message}`, "error");
  }
}

async function deleteRule(id) {
  const ok = confirm(`Delete rule ${id}?`);
  if (!ok) return;

  try {
    const data = await fetchJson(`/api/rules/${encodeURIComponent(id)}`, {
      method: "DELETE"
    });

    setRuleMessage(`Deleted rule ${data.id || id}`, "ok");

    if (editingRuleId === id) {
      clearRuleForm();
    }

    await loadRules();
  } catch (err) {
    console.error(err);
    setRuleMessage(`Delete failed: ${err.message}`, "error");
  }
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

  const condition = {
    field,
    op
  };

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

function connectStream() {
  if (eventSource) {
    eventSource.close();
  }

  statusText.textContent = "Connecting live stream...";
  eventSource = new EventSource("/stream");

  eventSource.addEventListener("open", () => {
    statusText.textContent = "Live";
  });

  eventSource.addEventListener("hello", () => {
    statusText.textContent = "Live";
  });

  eventSource.addEventListener("event", (e) => {
    try {
      const item = JSON.parse(e.data);
      addLiveEvent(item);
      statLastUpdate.textContent = new Date().toLocaleTimeString();
    } catch (err) {
      console.error("Failed to parse event SSE payload", err);
    }
  });

  eventSource.addEventListener("alert", (e) => {
    try {
      const item = JSON.parse(e.data);
      addLiveAlert(item);
      statLastUpdate.textContent = new Date().toLocaleTimeString();
    } catch (err) {
      console.error("Failed to parse alert SSE payload", err);
    }
  });

  eventSource.onerror = () => {
    statusText.textContent = "Disconnected";

    if (eventSource) {
      eventSource.close();
      eventSource = null;
    }

    allEvents = [];
    allAlerts = [];
    openedJson.clear();

    renderEvents([]);
    renderAlerts([]);
    updateStats([], []);
    statLastUpdate.textContent = "—";
  };
}

//fix web update-a
function toggleJson(id) {
  const el = document.getElementById(id);
  if (!el) return;

  el.classList.toggle("open");

  if (el.classList.contains("open")) {
    openedJson.add(id);
  } else {
    openedJson.delete(id);
  }
}

window.toggleJson = toggleJson;
window.editRule = editRule;
window.toggleRule = toggleRule;
window.deleteRule = deleteRule;
window.removeCondition = removeCondition;

eventTypeFilter.addEventListener("change", applyFilters);
severityFilter.addEventListener("change", applyFilters);
limitInput.addEventListener("change", loadData);

refreshBtn.addEventListener("click", async () => {
  await loadData();
  await loadRules();
});

reloadRulesBtn.addEventListener("click", loadRules);
loadRulesBtn.addEventListener("click", loadRules);
addConditionBtn.addEventListener("click", addCondition);
createRuleBtn.addEventListener("click", createRule);
updateRuleBtn.addEventListener("click", updateRule);
clearRuleFormBtn.addEventListener("click", clearRuleForm);

renderConditions();

Promise.all([loadData(), loadRules()]).then(() => {
  connectStream();
});
