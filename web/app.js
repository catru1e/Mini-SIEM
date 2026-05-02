const eventsList = document.getElementById("eventsList");
const alertsList = document.getElementById("alertsList");
const eventsCount = document.getElementById("eventsCount");
const alertsCount = document.getElementById("alertsCount");
const statusText = document.getElementById("statusText");

const statEvents = document.getElementById("statEvents");
const statAlerts = document.getElementById("statAlerts");
const statHighAlerts = document.getElementById("statHighAlerts");
const statLastUpdate = document.getElementById("statLastUpdate");

const eventTypeFilter = document.getElementById("eventTypeFilter");
const severityFilter = document.getElementById("severityFilter");
const limitInput = document.getElementById("limitInput");
const refreshBtn = document.getElementById("refreshBtn");

//fix web update-a
const openedJson = new Set();

let allEvents = [];
let allAlerts = [];
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
          ${createMetaItem("Severity", item.severity || "info")}
          ${createMetaItem("Event ID", item.id ?? "live")}
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
          ${createMetaItem("Type", "Detection/Correlation")}
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

async function fetchJson(url) {
  const response = await fetch(url);
  if (!response.ok) {
    throw new Error(`HTTP ${response.status} for ${url}`);
  }
  return response.json();
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

eventTypeFilter.addEventListener("change", applyFilters);
severityFilter.addEventListener("change", applyFilters);
limitInput.addEventListener("change", loadData);
refreshBtn.addEventListener("click", loadData);

loadData().then(() => {
  connectStream();
});
