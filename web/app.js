const $ = (id) => document.getElementById(id);

const statusText = $("statusText");
const runtimeNote = $("runtimeNote");
const lastUpdateText = $("lastUpdateText");
const pageTitle = $("pageTitle");
const pageSubtitle = $("pageSubtitle");

const refreshBtn = $("refreshBtn");
const autoRefreshBtn = $("autoRefreshBtn");
const themeToggleBtn = $("themeToggleBtn");
const loginThemeToggleBtn = $("loginThemeToggleBtn");

const sidebarToggleBtn = $("sidebarToggleBtn");
const sideBrandMark = $("sideBrandMark");

const clearOpenBtn = $("clearOpenBtn");
const limitInput = $("limitInput");

const loginScreen = $("loginScreen");
const appShell = $("appShell");
const loginForm = $("loginForm");
const loginUsernameInput = $("loginUsernameInput");
const loginPasswordInput = $("loginPasswordInput");
const loginMessage = $("loginMessage");

const currentUserText = $("currentUserText");
const currentRoleText = $("currentRoleText");
const logoutBtn = $("logoutBtn");

const changePasswordOpenBtn = $("changePasswordOpenBtn");
const passwordModal = $("passwordModal");
const passwordModalText = $("passwordModalText");
const passwordMessage = $("passwordMessage");
const oldPasswordInput = $("oldPasswordInput");
const newOwnPasswordInput = $("newOwnPasswordInput");
const repeatOwnPasswordInput = $("repeatOwnPasswordInput");
const changePasswordBtn = $("changePasswordBtn");
const cancelPasswordBtn = $("cancelPasswordBtn");

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

const usersCount = $("usersCount");
const userMessage = $("userMessage");
const usersList = $("usersList");

const newUsernameInput = $("newUsernameInput");
const newPasswordInput = $("newPasswordInput");
const newRoleInput = $("newRoleInput");
const newEnabledInput = $("newEnabledInput");
const createUserBtn = $("createUserBtn");
const clearUserFormBtn = $("clearUserFormBtn");

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

const editorModePill = $("editorModePill");
const ruleJsonPreview = $("ruleJsonPreview");
const ruleValidationBox = $("ruleValidationBox");
const validateRuleBtn = $("validateRuleBtn");
const copyRuleJsonBtn = $("copyRuleJsonBtn");

const receiversCount = $("receiversCount");
const receiverMessage = $("receiverMessage");
const newReceiverBtn = $("newReceiverBtn");
const receiversList = $("receiversList");

const deleteSelectedReceiverBtn = $("deleteSelectedReceiverBtn");
const openSelectedReceiverSettingsBtn = $("openSelectedReceiverSettingsBtn");
const receiverTreeSearchInput = $("receiverTreeSearchInput");

const receiverEmptyState = $("receiverEmptyState");
const receiverDetailPanel = $("receiverDetailPanel");
const receiverDetailTitle = $("receiverDetailTitle");
const receiverDetailSubtitle = $("receiverDetailSubtitle");
const openReceiverSettingsInlineBtn = $("openReceiverSettingsInlineBtn");

const receiverDashboardStandard = $("receiverDashboardStandard");
const receiverDashboardCustom = $("receiverDashboardCustom");
const receiverDashboardParsers = $("receiverDashboardParsers");

const receiverKpiEvents = $("receiverKpiEvents");
const receiverKpiUnknown = $("receiverKpiUnknown");
const receiverKpiParsed = $("receiverKpiParsed");
const receiverKpiParserRules = $("receiverKpiParserRules");

const receiverEventTypesWidget = $("receiverEventTypesWidget");
const receiverParserStatusWidget = $("receiverParserStatusWidget");
const receiverRecentEvents = $("receiverRecentEvents");

const receiverCustomDashboardsList = $("receiverCustomDashboardsList");
const saveReceiverDashboardsBtn = $("saveReceiverDashboardsBtn");

const receiverParserStatusFilterInput = $("receiverParserStatusFilterInput");
const receiverKqlFilterInput = $("receiverKqlFilterInput");
const clearReceiverFiltersBtn = $("clearReceiverFiltersBtn");

const receiverSettingsPanel = $("receiverSettingsPanel");
const receiverSettingsTitle = $("receiverSettingsTitle");
const receiverSettingsSubtitle = $("receiverSettingsSubtitle");
const closeReceiverSettingsBtn = $("closeReceiverSettingsBtn");

const receiverIdInput = $("receiverIdInput");
const receiverEnabledInput = $("receiverEnabledInput");
const receiverNameInput = $("receiverNameInput");
const receiverTypeInput = $("receiverTypeInput");

const receiverRoleInput = $("receiverRoleInput");
const receiverParentInput = $("receiverParentInput");

const receiverPathInput = $("receiverPathInput");
const receiverFilePatternInput = $("receiverFilePatternInput");
const receiverRecursiveInput = $("receiverRecursiveInput");
const receiverSourceTypeInput = $("receiverSourceTypeInput");
const receiverInitialPositionInput = $("receiverInitialPositionInput");
const receiverPolicyGroupsList = $("receiverPolicyGroupsList");

const saveReceiverBtn = $("saveReceiverBtn");
const deleteReceiverBtn = $("deleteReceiverBtn");

const receiverParserRulesList = $("receiverParserRulesList");

const newParserRuleFromSettingsBtn = $("newParserRuleFromSettingsBtn");
const deleteSelectedParserRuleBtn = $("deleteSelectedParserRuleBtn");
const editSelectedParserRuleBtn = $("editSelectedParserRuleBtn");

const toggleSelectedParserRuleBtn = $("toggleSelectedParserRuleBtn");

const parserRuleMessage = receiverMessage;
const parserRuleFormTitle = $("parserRuleFormTitle");
const parserEditorModePill = $("parserEditorModePill");

const parserRuleIdInput = $("parserRuleIdInput");
const parserRuleEnabledInput = $("parserRuleEnabledInput");
const parserRuleNameInput = $("parserRuleNameInput");
const parserPolicyGroupInput = $("parserPolicyGroupInput");
const parserMatchTypeInput = $("parserMatchTypeInput");

const parserRawSampleInput = $("parserRawSampleInput");
const parserPatternInput = $("parserPatternInput");
const parserFieldOrderInput = $("parserFieldOrderInput");

const parserConstantEventCodeInput = $("parserConstantEventCodeInput");
const parserConstantEventNameInput = $("parserConstantEventNameInput");
const parserConstantEventTypeInput = $("parserConstantEventTypeInput");
const parserConstantEventCategoryInput = $("parserConstantEventCategoryInput");
const parserConstantEventActionInput = $("parserConstantEventActionInput");
const parserConstantEventOutcomeInput = $("parserConstantEventOutcomeInput");
const parserConstantSeverityInput = $("parserConstantSeverityInput");
const parserConstantVendorInput = $("parserConstantVendorInput");
const parserConstantProductInput = $("parserConstantProductInput");

const testParserRuleBtn = $("testParserRuleBtn");
const copyParserRuleJsonBtn = $("copyParserRuleJsonBtn");
const clearParserRuleFormBtn = $("clearParserRuleFormBtn");

const parserExtractedPreview = $("parserExtractedPreview");
const parserMappingRows = $("parserMappingRows");
const parserReceiversList = $("parserReceiversList");

const createParserRuleBtn = $("createParserRuleBtn");
const updateParserRuleBtn = $("updateParserRuleBtn");

const parserRuleJsonPreview = $("parserRuleJsonPreview");

const dashboardOverview = $("dashboardOverview");
const dashboardAuth = $("dashboardAuth");
const dashboardFile = $("dashboardFile");

const topIpsWidget = $("topIpsWidget");
const topUsersWidget = $("topUsersWidget");
const alertsTrendWidget = $("alertsTrendWidget");

const authTotalEvents = $("authTotalEvents");
const authFailedEvents = $("authFailedEvents");
const authSuccessEvents = $("authSuccessEvents");
const authInvalidEvents = $("authInvalidEvents");
const authTopIpsWidget = $("authTopIpsWidget");
const authTopUsersWidget = $("authTopUsersWidget");
const authTypesWidget = $("authTypesWidget");
const authAlertsWidget = $("authAlertsWidget");
const authRecentEvents = $("authRecentEvents");

const fileTotalEvents = $("fileTotalEvents");
const fileCreatedEvents = $("fileCreatedEvents");
const fileModifiedEvents = $("fileModifiedEvents");
const fileDeletedEvents = $("fileDeletedEvents");
const fileTopPathsWidget = $("fileTopPathsWidget");
const fileActionsWidget = $("fileActionsWidget");
const fileSeverityWidget = $("fileSeverityWidget");
const fileAlertsWidget = $("fileAlertsWidget");
const fileRecentEvents = $("fileRecentEvents");
const dashboardCustom = $("dashboardCustom");

const newDashboardBtn = $("newDashboardBtn");
const duplicateDashboardBtn = $("duplicateDashboardBtn");
const saveDashboardBtn = $("saveDashboardBtn");
const deleteDashboardBtn = $("deleteDashboardBtn");

const customDashboardMessage = $("customDashboardMessage");
const customDashboardSelect = $("customDashboardSelect");
const customDashboardTitleInput = $("customDashboardTitleInput");
const customDashboardDescriptionInput = $("customDashboardDescriptionInput");
const customDashboardColumnsInput = $("customDashboardColumnsInput");

const widgetEditorModePill = $("widgetEditorModePill");
const widgetTypeInput = $("widgetTypeInput");
const widgetTitleInput = $("widgetTitleInput");
const widgetWidthInput = $("widgetWidthInput");
const widgetLimitInput = $("widgetLimitInput");
const widgetFilterSourceTypeInput = $("widgetFilterSourceTypeInput");
const widgetFilterSeverityInput = $("widgetFilterSeverityInput");
const widgetFilterEventTypeInput = $("widgetFilterEventTypeInput");
const widgetFilterFieldInput = $("widgetFilterFieldInput");
const widgetFilterValueInput = $("widgetFilterValueInput");

const addWidgetBtn = $("addWidgetBtn");
const updateWidgetBtn = $("updateWidgetBtn");
const clearWidgetFormBtn = $("clearWidgetFormBtn");

const customWidgetsList = $("customWidgetsList");
const customPreviewTitle = $("customPreviewTitle");
const customPreviewDescription = $("customPreviewDescription");
const customPreviewId = $("customPreviewId");
const customPreviewWidgetCount = $("customPreviewWidgetCount");
const customDashboardRender = $("customDashboardRender");
const customDashboardJsonPreview = $("customDashboardJsonPreview");


const dashboardLibraryCount = $("dashboardLibraryCount");
const dashboardLibraryList = $("dashboardLibraryList");

let customDashboards = [];
let currentCustomDashboard = null;
let selectedCustomDashboardId = "";
let editingWidgetId = "";

const widgetTypeLabels = {
  kpi_events: "KPI: events count",
  kpi_alerts: "KPI: alerts count",
  kpi_high_alerts: "KPI: high / critical alerts",
  top_ips: "Top source IPs",
  top_users: "Top users",
  top_file_paths: "Top file paths",
  alerts_trend: "Alerts trend",
  event_type_distribution: "Event type distribution",
  severity_distribution: "Severity distribution",
  source_type_distribution: "Source type distribution",
  recent_events: "Recent events",
  recent_alerts: "Recent alerts"
};

const defaultWidgetTitles = {
  kpi_events: "Events",
  kpi_alerts: "Alerts",
  kpi_high_alerts: "High / Critical alerts",
  top_ips: "Top source IPs",
  top_users: "Top users",
  top_file_paths: "Top file paths",
  alerts_trend: "Alerts trend",
  event_type_distribution: "Event type distribution",
  severity_distribution: "Severity distribution",
  source_type_distribution: "Source type distribution",
  recent_events: "Recent events",
  recent_alerts: "Recent alerts"
};

function makeUiId(prefix) {
  const random = Math.random().toString(16).slice(2, 8).toUpperCase();
  const time = Date.now().toString().slice(-6);
  return `${prefix}_${time}_${random}`;
}

function normalizeDashboard(raw) {
  const dashboard = cloneJson(raw || {});

  dashboard.id = String(dashboard.id || makeUiId("DASHBOARD"));
  dashboard.title = String(dashboard.title || "Untitled dashboard");
  dashboard.description = String(dashboard.description || "");
  dashboard.locked = dashboard.locked === true;

  if (!dashboard.layout || typeof dashboard.layout !== "object") {
    dashboard.layout = {};
  }

  const columns = Number(dashboard.layout.columns) || 2;
  dashboard.layout.columns = Math.max(1, Math.min(3, columns));

  if (!Array.isArray(dashboard.widgets)) {
    dashboard.widgets = [];
  }

  dashboard.widgets = dashboard.widgets.map(normalizeWidget);

  return dashboard;
}

function normalizeWidget(raw) {
  const widget = cloneJson(raw || {});

  widget.id = String(widget.id || makeUiId("WIDGET"));
  widget.type = String(widget.type || "kpi_events");
  widget.title = String(widget.title || defaultWidgetTitles[widget.type] || widget.type);
  widget.width = String(widget.width || "half");
  widget.limit = Math.max(1, Number(widget.limit) || 8);

  if (!widget.filters || typeof widget.filters !== "object") {
    widget.filters = {};
  }

  widget.filters = normalizeWidgetFilters(widget.filters);

  return widget;
}

function normalizeWidgetFilters(raw) {
  const filters = cloneJson(raw || {});

  return {
    source_type: String(filters.source_type || ""),
    severity: String(filters.severity || ""),
    event_type: String(filters.event_type || ""),
    field: String(filters.field || ""),
    value: String(filters.value || "")
  };
}

function createBlankDashboard() {
  return normalizeDashboard({
    id: makeUiId("DASHBOARD"),
    title: "New custom dashboard",
    description: "Custom investigation dashboard.",
    locked: false,
    layout: {
      columns: 2
    },
    widgets: []
  });
}

function setCustomDashboardMessage(message, type = "info") {
  if (!customDashboardMessage) return;

  customDashboardMessage.textContent = message || "";
  customDashboardMessage.className = `custom-dashboard-message ${type}`;
}

function setCurrentCustomDashboard(dashboard) {
  currentCustomDashboard = normalizeDashboard(dashboard);
  selectedCustomDashboardId = currentCustomDashboard.id;

  fillCustomDashboardForm();
  renderCustomDashboardSelect();
  renderCustomDashboardBuilder();
}

function currentDashboardFromForm() {
  const base = currentCustomDashboard
    ? normalizeDashboard(currentCustomDashboard)
    : createBlankDashboard();

  base.title = customDashboardTitleInput.value.trim() || "Untitled dashboard";
  base.description = customDashboardDescriptionInput.value.trim();
  base.layout = {
    columns: Math.max(1, Math.min(3, Number(customDashboardColumnsInput.value) || 2))
  };

  return normalizeDashboard(base);
}

function fillCustomDashboardForm() {
  if (!currentCustomDashboard) return;

  customDashboardTitleInput.value = currentCustomDashboard.title || "";
  customDashboardDescriptionInput.value = currentCustomDashboard.description || "";
  customDashboardColumnsInput.value = String(currentCustomDashboard.layout?.columns || 2);
}

function renderCustomDashboardSelect() {
  if (!customDashboardSelect) return;

  if (!customDashboards.length) {
    customDashboardSelect.innerHTML = `<option value="">No saved dashboards</option>`;
    return;
  }

  customDashboardSelect.innerHTML = customDashboards.map((dashboard) => `
    <option value="${escapeAttr(dashboard.id)}" ${dashboard.id === selectedCustomDashboardId ? "selected" : ""}>
      ${escapeHtml(dashboard.title || dashboard.id)}
    </option>
  `).join("");
}

function renderDashboardLibrary() {
  if (!dashboardLibraryList || !dashboardLibraryCount) return;

  dashboardLibraryCount.textContent = String(customDashboards.length);

  if (!customDashboards.length) {
    dashboardLibraryList.innerHTML = `
      <div class="empty small">
        No saved dashboards yet. Create a dashboard and click Save dashboard.
      </div>
    `;
    return;
  }

  dashboardLibraryList.innerHTML = customDashboards.map((dashboard) => {
    const active = currentCustomDashboard && currentCustomDashboard.id === dashboard.id;
    const widgetsCount = Array.isArray(dashboard.widgets) ? dashboard.widgets.length : 0;

    return `
      <article class="dashboard-library-item ${active ? "active" : ""}">
        <div class="dashboard-library-main">
          <strong>${escapeHtml(dashboard.title || dashboard.id)}</strong>
          <span>${escapeHtml(dashboard.description || "No description")}</span>

          <div class="dashboard-library-meta">
            <em>${escapeHtml(dashboard.id)}</em>
            <em>${widgetsCount} widgets</em>
            <em>${dashboard.layout?.columns || 2} columns</em>
          </div>
        </div>

        <div class="dashboard-library-actions">
          <button onclick="selectCustomDashboard('${escapeAttr(dashboard.id)}')">Open</button>
          <button onclick="duplicateDashboardById('${escapeAttr(dashboard.id)}')">Duplicate</button>
        </div>
      </article>
    `;
  }).join("");
}

function renderCustomDashboardSelect() {
  if (!customDashboardSelect) return;

  const options = [];

  if (currentCustomDashboard && !customDashboards.some((item) => item.id === currentCustomDashboard.id)) {
    options.push(`
      <option value="${escapeAttr(currentCustomDashboard.id)}" selected>
        ${escapeHtml(currentCustomDashboard.title || "Unsaved dashboard")} (unsaved)
      </option>
    `);
  }

  if (customDashboards.length) {
    options.push(...customDashboards.map((dashboard) => `
      <option value="${escapeAttr(dashboard.id)}" ${dashboard.id === selectedCustomDashboardId ? "selected" : ""}>
        ${escapeHtml(dashboard.title || dashboard.id)}
      </option>
    `));
  }

  if (!options.length) {
    options.push(`<option value="">No saved dashboards</option>`);
  }

  customDashboardSelect.innerHTML = options.join("");
}

async function loadCustomDashboards(options = {}) {
  const silent = options.silent === true;

  try {
    const data = await fetchJson("/api/dashboards");
    customDashboards = Array.isArray(data.dashboards)
      ? data.dashboards.map(normalizeDashboard)
      : [];

    const preferredId = selectedCustomDashboardId || localStorage.getItem("mini_siem_custom_dashboard_id") || "";
    const preferred = customDashboards.find((dashboard) => dashboard.id === preferredId);
    const first = customDashboards[0];

    if (preferred) {
      setCurrentCustomDashboard(preferred);
    } else if (first) {
      setCurrentCustomDashboard(first);
    } else {
      setCurrentCustomDashboard(createBlankDashboard());
    }

    if (!silent) {
      setCustomDashboardMessage("Custom dashboards loaded", "ok");
    }
  } catch (err) {
    console.error(err);

    if (!currentCustomDashboard) {
      setCurrentCustomDashboard(createBlankDashboard());
    }

    setCustomDashboardMessage(`Failed to load dashboards: ${err.message}`, "error");
  }
}

function selectCustomDashboard(id) {
  const dashboard = customDashboards.find((item) => item.id === id);

  if (!dashboard) {
    setCustomDashboardMessage(`Dashboard not found: ${id}`, "error");
    return;
  }

  setCurrentCustomDashboard(dashboard);
  localStorage.setItem("mini_siem_custom_dashboard_id", dashboard.id);
  clearWidgetForm();
  setCustomDashboardMessage(`Selected ${dashboard.title}`, "info");
}

function newCustomDashboard() {
  setCurrentCustomDashboard(createBlankDashboard());
  clearWidgetForm();
  setCustomDashboardMessage("New unsaved dashboard created", "info");
}

function duplicateCustomDashboard() {
  if (!currentCustomDashboard) {
    setCustomDashboardMessage("No dashboard selected", "error");
    return;
  }

  const copy = normalizeDashboard(currentCustomDashboard);
  copy.id = makeUiId("DASHBOARD");
  copy.title = `${copy.title} Copy`;
  copy.locked = false;
  copy.created_at = "";
  copy.updated_at = "";
  copy.widgets = copy.widgets.map((widget) => ({
    ...normalizeWidget(widget),
    id: makeUiId("WIDGET")
  }));

  setCurrentCustomDashboard(copy);
  clearWidgetForm();
  setCustomDashboardMessage("Dashboard duplicated. Click Save dashboard to persist it.", "ok");
}

function duplicateDashboardById(id) {
  const dashboard = customDashboards.find((item) => item.id === id);

  if (!dashboard) {
    setCustomDashboardMessage(`Dashboard not found: ${id}`, "error");
    return;
  }

  const copy = normalizeDashboard(dashboard);
  copy.id = makeUiId("DASHBOARD");
  copy.title = `${copy.title} Copy`;
  copy.locked = false;
  copy.created_at = "";
  copy.updated_at = "";
  copy.widgets = copy.widgets.map((widget) => ({
    ...normalizeWidget(widget),
    id: makeUiId("WIDGET")
  }));

  setCurrentCustomDashboard(copy);
  clearWidgetForm();
  activateDashboardMode("custom");

  setCustomDashboardMessage("Dashboard duplicated. Click Save dashboard to persist it.", "ok");
}

async function saveCustomDashboard() {
  try {
    if (!currentCustomDashboard) {
      throw new Error("No dashboard selected");
    }

    currentCustomDashboard = currentDashboardFromForm();

    if (!currentCustomDashboard.title.trim()) {
      throw new Error("Dashboard title is required");
    }

    if (!Array.isArray(currentCustomDashboard.widgets)) {
      currentCustomDashboard.widgets = [];
    }

    const exists = customDashboards.some((dashboard) => dashboard.id === currentCustomDashboard.id);
    const method = exists ? "PUT" : "POST";
    const url = exists
      ? `/api/dashboards/${encodeURIComponent(currentCustomDashboard.id)}`
      : "/api/dashboards";

    const data = await fetchJson(url, {
      method,
      headers: {
        "Content-Type": "application/json"
      },
      body: JSON.stringify(currentCustomDashboard)
    });

    const saved = normalizeDashboard(data.dashboard || currentCustomDashboard);

    const existingIndex = customDashboards.findIndex((dashboard) => dashboard.id === saved.id);
    if (existingIndex >= 0) {
      customDashboards[existingIndex] = saved;
    } else {
      customDashboards.push(saved);
    }

    setCurrentCustomDashboard(saved);
    localStorage.setItem("mini_siem_custom_dashboard_id", saved.id);

    setCustomDashboardMessage(`Saved dashboard: ${saved.title}`, "ok");
  } catch (err) {
    setCustomDashboardMessage(`Save failed: ${err.message}`, "error");
  }
}

async function deleteCustomDashboard() {
  try {
    if (!currentCustomDashboard) {
      throw new Error("No dashboard selected");
    }

    const exists = customDashboards.some((dashboard) => dashboard.id === currentCustomDashboard.id);

    if (!exists) {
      setCurrentCustomDashboard(createBlankDashboard());
      clearWidgetForm();
      setCustomDashboardMessage("Unsaved dashboard discarded", "info");
      return;
    }

    if (currentCustomDashboard.locked) {
      throw new Error("Locked dashboard cannot be deleted");
    }

    if (!confirm(`Delete dashboard "${currentCustomDashboard.title}"?`)) {
      return;
    }

    await fetchJson(`/api/dashboards/${encodeURIComponent(currentCustomDashboard.id)}`, {
      method: "DELETE"
    });

    customDashboards = customDashboards.filter((dashboard) => dashboard.id !== currentCustomDashboard.id);

    const next = customDashboards[0] || createBlankDashboard();
    setCurrentCustomDashboard(next);
    clearWidgetForm();

    setCustomDashboardMessage("Dashboard deleted", "ok");
  } catch (err) {
    setCustomDashboardMessage(`Delete failed: ${err.message}`, "error");
  }
}

function widgetFiltersFromForm() {
  return normalizeWidgetFilters({
    source_type: widgetFilterSourceTypeInput.value,
    severity: widgetFilterSeverityInput.value,
    event_type: widgetFilterEventTypeInput.value.trim(),
    field: widgetFilterFieldInput.value.trim(),
    value: widgetFilterValueInput.value.trim()
  });
}

function widgetFromForm() {
  const type = widgetTypeInput.value || "kpi_events";
  const title = widgetTitleInput.value.trim() || defaultWidgetTitles[type] || type;

  return normalizeWidget({
    id: editingWidgetId || makeUiId("WIDGET"),
    type,
    title,
    width: widgetWidthInput.value || "half",
    limit: Math.max(1, Math.min(100, Number(widgetLimitInput.value) || 8)),
    filters: widgetFiltersFromForm()
  });
}

function fillWidgetForm(widget) {
  const normalized = normalizeWidget(widget);

  editingWidgetId = normalized.id;

  widgetTypeInput.value = normalized.type;
  widgetTitleInput.value = normalized.title;
  widgetWidthInput.value = normalized.width;
  widgetLimitInput.value = String(normalized.limit || 8);

  widgetFilterSourceTypeInput.value = normalized.filters.source_type || "";
  widgetFilterSeverityInput.value = normalized.filters.severity || "";
  widgetFilterEventTypeInput.value = normalized.filters.event_type || "";
  widgetFilterFieldInput.value = normalized.filters.field || "";
  widgetFilterValueInput.value = normalized.filters.value || "";

  addWidgetBtn.disabled = true;
  updateWidgetBtn.disabled = false;

  if (widgetEditorModePill) {
    widgetEditorModePill.textContent = "edit mode";
  }

  setCustomDashboardMessage(`Editing widget: ${normalized.title}`, "info");
}

function clearWidgetForm() {
  editingWidgetId = "";

  widgetTypeInput.value = "kpi_events";
  widgetTitleInput.value = "";
  widgetWidthInput.value = "half";
  widgetLimitInput.value = "8";

  widgetFilterSourceTypeInput.value = "";
  widgetFilterSeverityInput.value = "";
  widgetFilterEventTypeInput.value = "";
  widgetFilterFieldInput.value = "";
  widgetFilterValueInput.value = "";

  addWidgetBtn.disabled = false;
  updateWidgetBtn.disabled = true;

  if (widgetEditorModePill) {
    widgetEditorModePill.textContent = "add mode";
  }
}

function addCustomWidget() {
  if (!currentCustomDashboard) {
    setCurrentCustomDashboard(createBlankDashboard());
  }

  const dashboard = currentDashboardFromForm();
  const widget = widgetFromForm();

  dashboard.widgets.push(widget);
  currentCustomDashboard = normalizeDashboard(dashboard);

  clearWidgetForm();
  renderCustomDashboardBuilder();

  setCustomDashboardMessage(`Widget added: ${widget.title}`, "ok");
}

function updateCustomWidget() {
  if (!currentCustomDashboard) {
    setCustomDashboardMessage("No dashboard selected", "error");
    return;
  }

  if (!editingWidgetId) {
    setCustomDashboardMessage("No widget selected", "error");
    return;
  }

  const dashboard = currentDashboardFromForm();
  const index = dashboard.widgets.findIndex((widget) => widget.id === editingWidgetId);

  if (index < 0) {
    setCustomDashboardMessage("Widget not found", "error");
    return;
  }

  dashboard.widgets[index] = widgetFromForm();
  currentCustomDashboard = normalizeDashboard(dashboard);

  clearWidgetForm();
  renderCustomDashboardBuilder();

  setCustomDashboardMessage("Widget updated", "ok");
}

function editCustomWidget(id) {
  if (!currentCustomDashboard) return;

  const widget = currentCustomDashboard.widgets.find((item) => item.id === id);

  if (!widget) {
    setCustomDashboardMessage(`Widget not found: ${id}`, "error");
    return;
  }

  fillWidgetForm(widget);
}

function removeCustomWidget(id) {
  if (!currentCustomDashboard) return;

  const dashboard = currentDashboardFromForm();
  dashboard.widgets = dashboard.widgets.filter((widget) => widget.id !== id);

  currentCustomDashboard = normalizeDashboard(dashboard);

  if (editingWidgetId === id) {
    clearWidgetForm();
  }

  renderCustomDashboardBuilder();
  setCustomDashboardMessage("Widget removed", "ok");
}

function moveCustomWidget(id, direction) {
  if (!currentCustomDashboard) return;

  const dashboard = currentDashboardFromForm();
  const index = dashboard.widgets.findIndex((widget) => widget.id === id);

  if (index < 0) return;

  const nextIndex = index + direction;

  if (nextIndex < 0 || nextIndex >= dashboard.widgets.length) {
    return;
  }

  const tmp = dashboard.widgets[index];
  dashboard.widgets[index] = dashboard.widgets[nextIndex];
  dashboard.widgets[nextIndex] = tmp;

  currentCustomDashboard = normalizeDashboard(dashboard);
  renderCustomDashboardBuilder();
}
function widgetFilterMatchesField(item, filters, isAlert = false) {
  const field = String(filters.field || "").trim();
  const value = lower(filters.value || "");

  if (!field || !value) {
    return true;
  }

  const actual = isAlert
    ? alertField(item, field)
    : eventField(item, field);

  return lower(actual).includes(value);
}

function applyEventWidgetFilters(items, filters = {}) {
  const normalized = normalizeWidgetFilters(filters);

  return (items || []).filter((item) => {
    if (normalized.source_type && lower(eventField(item, "source_type")) !== lower(normalized.source_type)) {
      return false;
    }

    if (normalized.severity && lower(eventField(item, "severity")) !== lower(normalized.severity)) {
      return false;
    }

    if (normalized.event_type && lower(eventField(item, "event_type")) !== lower(normalized.event_type)) {
      return false;
    }

    if (!widgetFilterMatchesField(item, normalized, false)) {
      return false;
    }

    return true;
  });
}

function applyAlertWidgetFilters(items, filters = {}) {
  const normalized = normalizeWidgetFilters(filters);

  return (items || []).filter((item) => {
    if (normalized.source_type && lower(alertField(item, "source_type")) !== lower(normalized.source_type)) {
      return false;
    }

    if (normalized.severity && lower(alertField(item, "severity")) !== lower(normalized.severity)) {
      return false;
    }

    if (normalized.event_type) {
      const alert = alertPayload(item);
      if (lower(alert.event_type || "") !== lower(normalized.event_type)) {
        return false;
      }
    }

    if (!widgetFilterMatchesField(item, normalized, true)) {
      return false;
    }

    return true;
  });
}

function customWidgetSubtitle(widget) {
  const filters = normalizeWidgetFilters(widget.filters || {});
  const active = [];

  if (filters.source_type) active.push(`source_type:${filters.source_type}`);
  if (filters.severity) active.push(`severity:${filters.severity}`);
  if (filters.event_type) active.push(`event_type:${filters.event_type}`);
  if (filters.field && filters.value) active.push(`${filters.field}:${filters.value}`);

  if (!active.length) {
    return widgetTypeLabels[widget.type] || widget.type;
  }

  return active.join(" · ");
}

function customWidgetWidthClass(widget) {
  const width = String(widget.width || "half");

  if (width === "full") return "widget-full";
  if (width === "third") return "widget-third";

  return "widget-half";
}

function renderCustomKpiWidget(widget, value, caption) {
  return `
    <div class="custom-kpi-widget">
      <span>${escapeHtml(caption)}</span>
      <strong>${formatNumber(value)}</strong>
      <em>${escapeHtml(customWidgetSubtitle(widget))}</em>
    </div>
  `;
}

function renderCustomWidgetContent(widget) {
  const normalized = normalizeWidget(widget);
  const type = normalized.type;
  const limit = normalized.limit || 8;
  const eventItems = applyEventWidgetFilters(allEvents, normalized.filters);
  const alertItems = applyAlertWidgetFilters(allAlerts, normalized.filters);

  if (type === "kpi_events") {
    return renderCustomKpiWidget(normalized, eventItems.length, "matching events");
  }

  if (type === "kpi_alerts") {
    return renderCustomKpiWidget(normalized, alertItems.length, "matching alerts");
  }

  if (type === "kpi_high_alerts") {
    const count = alertItems.filter((item) => {
      const severity = lower(alertField(item, "severity"));
      return severity === "high" || severity === "critical";
    }).length;

    return renderCustomKpiWidget(normalized, count, "high / critical alerts");
  }

  if (type === "top_ips") {
    return `<div class="rank-list custom-widget-rank" id="custom-widget-body-${escapeAttr(normalized.id)}"></div>`;
  }

  if (type === "top_users") {
    return `<div class="rank-list custom-widget-rank" id="custom-widget-body-${escapeAttr(normalized.id)}"></div>`;
  }

  if (type === "top_file_paths") {
    return `<div class="rank-list custom-widget-rank" id="custom-widget-body-${escapeAttr(normalized.id)}"></div>`;
  }

  if (type === "alerts_trend") {
    return `<div class="timeline-chart compact" id="custom-widget-body-${escapeAttr(normalized.id)}"></div>`;
  }

  if (type === "event_type_distribution") {
    return `<div class="bars" id="custom-widget-body-${escapeAttr(normalized.id)}"></div>`;
  }

  if (type === "severity_distribution") {
    return `<div class="bars" id="custom-widget-body-${escapeAttr(normalized.id)}"></div>`;
  }

  if (type === "source_type_distribution") {
    return `<div class="bars" id="custom-widget-body-${escapeAttr(normalized.id)}"></div>`;
  }

  if (type === "recent_events") {
    const scope = `custom-events-${normalized.id}`;
    const recent = eventItems.slice(0, limit);
    eventPools[scope] = recent;

    return recent
      .map((item, index) => renderEventCard(item, index, scope))
      .join("") || `<div class="empty">No matching events</div>`;
  }

  if (type === "recent_alerts") {
    const scope = `custom-alerts-${normalized.id}`;
    const recent = alertItems.slice(0, limit);
    alertPools[scope] = recent;

    return recent
      .map((item, index) => renderAlertCard(item, index, scope))
      .join("") || `<div class="empty">No matching alerts</div>`;
  }

  return `<div class="empty">Unsupported widget type: ${escapeHtml(type)}</div>`;
}

function hydrateCustomWidgetBody(widget) {
  const normalized = normalizeWidget(widget);
  const target = $(`custom-widget-body-${normalized.id}`);
  if (!target) return;

  const eventItems = applyEventWidgetFilters(allEvents, normalized.filters);
  const alertItems = applyAlertWidgetFilters(allAlerts, normalized.filters);
  const limit = normalized.limit || 8;

  if (normalized.type === "top_ips") {
    renderRankList(
      target,
      topEntries(eventItems, (item) => eventField(item, "src_ip"), limit),
      "No source IPs found"
    );
    return;
  }

  if (normalized.type === "top_users") {
    renderRankList(
      target,
      topEntries(eventItems, (item) => eventField(item, "user"), limit),
      "No users found"
    );
    return;
  }

  if (normalized.type === "top_file_paths") {
    renderRankList(
      target,
      topEntries(eventItems, (item) => eventField(item, "path"), limit),
      "No file paths found"
    );
    return;
  }

  if (normalized.type === "alerts_trend") {
    renderAlertsTrend(target, alertItems.slice(0, limit));
    return;
  }

  if (normalized.type === "event_type_distribution") {
    renderBars(
      target,
      countBy(eventItems, (item) => eventField(item, "event_type")),
      false
    );
    return;
  }

  if (normalized.type === "severity_distribution") {
    const eventCounts = countBy(eventItems, (item) => eventField(item, "severity"));
    const alertCounts = countBy(alertItems, (item) => alertField(item, "severity"));
    const merged = { ...eventCounts };

    for (const [key, value] of Object.entries(alertCounts)) {
      merged[key] = (merged[key] || 0) + value;
    }

    renderBars(target, merged, true);
    return;
  }

  if (normalized.type === "source_type_distribution") {
    renderBars(
      target,
      countBy(eventItems, (item) => eventField(item, "source_type")),
      false
    );
  }
}

function renderCustomDashboardWidget(widget, index) {
  const normalized = normalizeWidget(widget);

  return `
    <article class="custom-widget-card ${customWidgetWidthClass(normalized)} widget-type-${escapeAttr(normalized.type)}" data-widget-type="${escapeAttr(normalized.type)}">
      <div class="custom-widget-head">
        <div>
          <div class="card-kicker">${escapeHtml(widgetTypeLabels[normalized.type] || normalized.type)}</div>
          <h4>${escapeHtml(normalized.title)}</h4>
          <p>${escapeHtml(customWidgetSubtitle(normalized))}</p>
        </div>

        <span class="custom-widget-size">${escapeHtml(normalized.width)}</span>
      </div>

      <div class="custom-widget-body">
        ${renderCustomWidgetContent(normalized)}
      </div>
    </article>
  `;
}

function renderCustomDashboardPreview() {
  if (!currentCustomDashboard || !customDashboardRender) return;

  const dashboard = normalizeDashboard(currentCustomDashboard);
  const columns = dashboard.layout?.columns || 2;

  customPreviewTitle.textContent = dashboard.title || "Custom dashboard";
  customPreviewDescription.textContent = dashboard.description || "No description";
  customPreviewId.textContent = dashboard.id || "—";
  customPreviewWidgetCount.textContent = `${dashboard.widgets.length} widgets`;

  customDashboardRender.className = `custom-dashboard-render columns-${columns}`;

  if (!dashboard.widgets.length) {
    customDashboardRender.innerHTML = `
      <div class="empty custom-empty-dashboard">
        No widgets yet. Use the Widget builder on the left to add KPI, ranking, trend or investigation widgets.
      </div>
    `;
  } else {
    customDashboardRender.innerHTML = dashboard.widgets
      .map((widget, index) => renderCustomDashboardWidget(widget, index))
      .join("");

    dashboard.widgets.forEach(hydrateCustomWidgetBody);
  }

  customDashboardJsonPreview.textContent = prettyJson(dashboard);
}

function renderCustomWidgetsList() {
  if (!customWidgetsList) return;

  if (!currentCustomDashboard || !currentCustomDashboard.widgets.length) {
    customWidgetsList.innerHTML = `<div class="empty small">No widgets configured</div>`;
    return;
  }

  customWidgetsList.innerHTML = currentCustomDashboard.widgets.map((widget, index) => {
    const normalized = normalizeWidget(widget);

    return `
      <article class="custom-widget-list-item">
        <div class="custom-widget-list-main">
          <strong>${escapeHtml(normalized.title)}</strong>
          <span>${escapeHtml(widgetTypeLabels[normalized.type] || normalized.type)} · ${escapeHtml(normalized.width)} · limit ${escapeHtml(normalized.limit)}</span>
        </div>

        <div class="custom-widget-list-actions">
          <button onclick="moveCustomWidget('${escapeAttr(normalized.id)}', -1)" ${index === 0 ? "disabled" : ""}>Up</button>
          <button onclick="moveCustomWidget('${escapeAttr(normalized.id)}', 1)" ${index === currentCustomDashboard.widgets.length - 1 ? "disabled" : ""}>Down</button>
          <button onclick="editCustomWidget('${escapeAttr(normalized.id)}')">Edit</button>
          <button class="delete" onclick="removeCustomWidget('${escapeAttr(normalized.id)}')">Remove</button>
        </div>
      </article>
    `;
  }).join("");
}

function renderCustomDashboardBuilder() {
  if (!currentCustomDashboard) {
    setCurrentCustomDashboard(createBlankDashboard());
    return;
  }

  currentCustomDashboard = normalizeDashboard(currentCustomDashboard);

  renderCustomDashboardSelect();
  renderDashboardLibrary();
  renderCustomWidgetsList();
  renderCustomDashboardPreview();
}

let currentDashboardMode = localStorage.getItem("mini_siem_dashboard_mode") || "overview";

function eventPayload(item) {
  return item?.event || {};
}

function alertPayload(item) {
  return item?.alert || {};
}

function firstValue(...values) {
  for (const value of values) {
    if (value !== undefined && value !== null && value !== "") {
      return value;
    }
  }

  return "";
}

function eventField(item, field) {
  const ev = eventPayload(item);

  if (field === "src_ip") {
    return firstValue(ev.src_ip, ev.ip, ev.source_ip);
  }

  if (field === "user") {
    return firstValue(ev.user, ev.username, ev.account);
  }

  if (field === "path") {
    return firstValue(ev.path, ev.watched_path, ev.file_path, ev.target_path);
  }

  if (field === "program") {
    return firstValue(ev.program, ev.process, ev.process_name);
  }

  if (field === "event_type") {
    return firstValue(item.event_type, ev.event_type);
  }

  if (field === "source_type") {
    return firstValue(item.source_type, ev.source_type);
  }

  if (field === "severity") {
    return firstValue(item.severity, ev.severity);
  }

  return firstValue(ev[field], item[field]);
}

function alertField(item, field) {
  const alert = alertPayload(item);

  if (field === "source_type") {
    return firstValue(alert.source_type, item.source_type);
  }

  if (field === "severity") {
    return firstValue(item.severity, alert.severity);
  }

  if (field === "rule") {
    return firstValue(item.rule_name, alert.rule_name, alert.rule_id);
  }

  if (field === "title") {
    return firstValue(item.title, alert.title);
  }

  if (field === "ts") {
    return firstValue(item.ts, alert.ts);
  }

  return firstValue(alert[field], item[field]);
}

function eventSourceType(item) {
  return lower(eventField(item, "source_type"));
}

function eventType(item) {
  return lower(eventField(item, "event_type"));
}

function alertSourceType(item) {
  return lower(alertField(item, "source_type"));
}

function isAuthEvent(item) {
  const sourceType = eventSourceType(item);
  const type = eventType(item);

  return (
    sourceType === "auth" ||
    type.startsWith("auth_") ||
    type === "failed_login" ||
    type === "accepted_login" ||
    type === "invalid_user" ||
    type === "privilege_escalation" ||
    type === "session_open" ||
    type === "session_close"
  );
}

function isFileEvent(item) {
  const sourceType = eventSourceType(item);
  const type = eventType(item);

  return (
    sourceType === "file" ||
    type.startsWith("file_") ||
    Boolean(eventField(item, "path")) ||
    Boolean(eventPayload(item).watched_path)
  );
}

function isAuthAlert(item) {
  const alert = alertPayload(item);
  const sourceType = alertSourceType(item);
  const type = lower(alert.event_type || "");
  const rule = lower(alertField(item, "rule"));

  return (
    sourceType === "auth" ||
    type.startsWith("auth_") ||
    rule.includes("auth") ||
    rule.includes("login") ||
    rule.includes("sudo") ||
    rule.includes("invalid_user")
  );
}

function isFileAlert(item) {
  const alert = alertPayload(item);
  const sourceType = alertSourceType(item);
  const type = lower(alert.event_type || "");
  const rule = lower(alertField(item, "rule"));

  return (
    sourceType === "file" ||
    type.startsWith("file_") ||
    Boolean(alert.path) ||
    rule.includes("file") ||
    rule.includes("ssh")
  );
}

function countBy(items, getter) {
  const counts = {};

  for (const item of items || []) {
    const rawKey = getter(item);
    const key = String(rawKey || "").trim();

    if (!key) continue;

    counts[key] = (counts[key] || 0) + 1;
  }

  return counts;
}

function topEntries(items, getter, limit = 8) {
  return Object.entries(countBy(items, getter))
    .map(([key, count]) => ({ key, count }))
    .sort((a, b) => b.count - a.count)
    .slice(0, limit);
}

function renderRankList(target, entries, emptyText = "No data") {
  if (!target) return;

  if (!entries.length) {
    target.innerHTML = `<div class="empty">${escapeHtml(emptyText)}</div>`;
    return;
  }

  const max = Math.max(...entries.map((item) => item.count), 1);

  target.innerHTML = entries.map((item, index) => {
    const width = Math.max(5, Math.round((item.count / max) * 100));

    return `
      <div class="rank-row">
        <div class="rank-index">${index + 1}</div>

        <div class="rank-main">
          <div class="rank-top">
            <strong>${escapeHtml(item.key)}</strong>
            <span>${formatNumber(item.count)}</span>
          </div>

          <div class="rank-track">
            <div class="rank-fill" style="width:${width}%"></div>
          </div>
        </div>
      </div>
    `;
  }).join("");
}

function getTimeBucket(value) {
  const raw = String(value || "");
  if (!raw) return "unknown";

  if (raw.length >= 16) {
    return raw.slice(0, 16);
  }

  return raw;
}

function renderAlertsTrend(target, alerts) {
  if (!target) return;

  const grouped = countBy(alerts, (item) => getTimeBucket(alertField(item, "ts")));
  const points = Object.entries(grouped)
    .map(([bucket, count]) => ({ bucket, count }))
    .sort((a, b) => a.bucket.localeCompare(b.bucket))
    .slice(-40);

  renderTimeline(target, points);
}

function setTextSafe(el, value) {
  if (!el) return;
  el.textContent = String(value);
}

function activateDashboardMode(mode) {
  currentDashboardMode = mode || "overview";
  localStorage.setItem("mini_siem_dashboard_mode", currentDashboardMode);

  document.querySelectorAll(".dashboard-mode-btn").forEach((btn) => {
    btn.classList.toggle("active", btn.dataset.dashboardMode === currentDashboardMode);
  });

  const views = {
    overview: dashboardOverview,
    auth: dashboardAuth,
    file: dashboardFile,
    custom: dashboardCustom
  };

  Object.entries(views).forEach(([key, view]) => {
    if (!view) return;
    view.classList.toggle("active", key === currentDashboardMode);
  });

  if (currentDashboardMode === "custom") {
    if (!currentCustomDashboard) {
      if (customDashboards.length) {
        setCurrentCustomDashboard(customDashboards[0]);
      } else {
        setCurrentCustomDashboard(createBlankDashboard());
      }
    }

    renderCustomDashboardBuilder();
  }
}

let stats = {};
let allEvents = [];
let allAlerts = [];
let allRules = [];

let allReceivers = [];
let allPolicyGroups = [];
let allParserRules = [];
let fieldCatalog = [];

let selectedReceiverId = "";
let editingReceiverId = "";
let currentReceiverSettingsTab = "config";

let currentReceiverDashboardTab = "standard";
let currentReceiverParserStatusFilter = "";
let currentReceiverKqlFilter = "";
let receiverTreeSearch = "";

let receiverTreeExpanded = new Set(
  JSON.parse(localStorage.getItem("mini_siem_receiver_tree_expanded") || "[]")
);

let currentParserExtracted = {};
let currentParserMapping = {};
let editingParserRuleId = "";
let selectedParserRuleId = "";

let currentConditions = [];
let editingRuleId = "";

let autoRefreshEnabled = false;
let autoRefreshTimer = null;
let currentTheme = localStorage.getItem("mini_siem_theme") || "horror";

let sidebarCollapsed = localStorage.getItem("mini_siem_sidebar_collapsed") === "true";

let currentUser = null;
let allUsers = [];

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
  receivers: {
    title: "Receivers",
    subtitle: "Manage log sources, receiver settings and parser rule rollout."
  },
  rules: {
    title: "Visual Detection Rule Editor",
    subtitle: "Create, validate, preview and save JSON-based SIEM detection rules."
  },
  users: {
    title: "User Management",
    subtitle: "Admin console for local Mini SIEM accounts and roles."
  }
};

const ruleTemplates = {
  failed_login_ip: {
    id: "RULE_FAILED_LOGIN_IP_CUSTOM",
    enabled: true,
    type: "threshold",
    event_types: ["auth_failed"],
    source_types: ["auth"],
    group_by: "src_ip",
    threshold: 5,
    window_sec: 300,
    suppress_sec: 60,
    severity: "high",
    title: "Brute force from IP",
    description: "Detected {{count}} failed login attempts from IP {{group_key}} within {{window_sec}} seconds.",
    conditions: []
  },

  failed_login_user: {
    id: "RULE_FAILED_LOGIN_USER_CUSTOM",
    enabled: true,
    type: "threshold",
    event_types: ["auth_failed"],
    source_types: ["auth"],
    group_by: "user",
    threshold: 5,
    window_sec: 300,
    suppress_sec: 60,
    severity: "high",
    title: "Multiple failed logins for user",
    description: "Detected {{count}} failed login attempts for user {{group_key}} within {{window_sec}} seconds.",
    conditions: []
  },

  syslog_contains: {
    id: "RULE_SYSLOG_RAW_CONTAINS_CUSTOM",
    enabled: true,
    type: "match",
    event_types: ["system_event"],
    source_types: ["syslog"],
    group_by: "program",
    threshold: 1,
    window_sec: 60,
    suppress_sec: 60,
    severity: "medium",
    title: "Syslog raw pattern detected",
    description: "Detected syslog pattern from {{group_key}}.",
    conditions: [
      {
        field: "raw",
        op: "contains",
        value: "error"
      }
    ]
  },

  blacklisted_process: {
    id: "RULE_BLACKLISTED_PROCESS_CUSTOM",
    enabled: true,
    type: "match",
    event_types: ["process_start"],
    source_types: ["process"],
    group_by: "process_name",
    threshold: 1,
    window_sec: 60,
    suppress_sec: 60,
    severity: "high",
    title: "Blacklisted process started",
    description: "Detected blacklisted process: {{group_key}}.",
    conditions: [
      {
        field: "process_name",
        op: "in",
        values: ["nc", "netcat", "ncat", "socat"]
      }
    ]
  },

  sensitive_file: {
    id: "RULE_SENSITIVE_FILE_CHANGE_CUSTOM",
    enabled: true,
    type: "match",
    event_types: ["file_created", "file_modified", "file_deleted"],
    source_types: ["file"],
    group_by: "path",
    threshold: 1,
    window_sec: 60,
    suppress_sec: 60,
    severity: "high",
    title: "Sensitive file changed",
    description: "Detected sensitive file activity: {{event_type}} on {{group_key}}.",
    conditions: [
      {
        field: "path",
        op: "contains_any",
        values: ["authorized_keys", "known_hosts", "config", "id_rsa", "id_ed25519"]
      }
    ]
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

function userInitial() {
  const username = currentUser?.username || currentUser?.name || "";

  if (!username) {
    return "MS";
  }

  return username.trim().slice(0, 1).toUpperCase();
}

function updateSideBrandMark() {
  if (!sideBrandMark) return;
  sideBrandMark.textContent = userInitial();
}

function applySidebarState() {
  if (!appShell) return;

  appShell.classList.toggle("sidebar-collapsed", sidebarCollapsed);
  localStorage.setItem("mini_siem_sidebar_collapsed", sidebarCollapsed ? "true" : "false");
}

function toggleSidebar() {
  sidebarCollapsed = !sidebarCollapsed;
  applySidebarState();
}

function setStatus(text) {
  statusText.textContent = text;
}

function touchUpdateTime() {
  lastUpdateText.textContent = nowTime();
}

function getLimit() {
  const value = Number(limitInput.value) || 1000000;
  return Math.max(1, value);
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

function cloneJson(value) {
  return JSON.parse(JSON.stringify(value));
}

async function fetchJson(url, options = {}) {
  const res = await fetch(url, {
    credentials: "same-origin",
    ...options,
    headers: {
      ...(options.headers || {})
    }
  });

  let data = null;

  try {
    data = await res.json();
  } catch (_) {
    data = null;
  }

  if (res.status === 401) {
    showLogin();
    throw new Error(data?.message || "login required");
  }

  if (!res.ok) {
    throw new Error(data?.message || `HTTP ${res.status}`);
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
  return `
    <div class="copy-pre-wrap">
      <button class="copy-pre-btn" onclick="copyPreBlock(this)">Copy</button>
      <pre class="detail-pre">${escapeHtml(value)}</pre>
    </div>
  `;
}
async function copyPreBlock(button) {
  try {
    const wrap = button.closest(".copy-pre-wrap");
    const pre = wrap?.querySelector("pre");

    if (!pre) {
      throw new Error("Nothing to copy");
    }

    const text = pre.textContent || "";

    if (navigator.clipboard?.writeText) {
      await navigator.clipboard.writeText(text);
    } else {
      const tmp = document.createElement("textarea");
      tmp.value = text;
      document.body.appendChild(tmp);
      tmp.select();
      document.execCommand("copy");
      tmp.remove();
    }

    const oldText = button.textContent;
    button.textContent = "Copied";
    button.classList.add("copied");

    setTimeout(() => {
      button.textContent = oldText;
      button.classList.remove("copied");
    }, 1200);
  } catch (err) {
    console.error(err);

    const oldText = button.textContent;
    button.textContent = "Failed";

    setTimeout(() => {
      button.textContent = oldText;
    }, 1200);
  }
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

function saveReceiverTreeExpanded() {
  localStorage.setItem(
    "mini_siem_receiver_tree_expanded",
    JSON.stringify(Array.from(receiverTreeExpanded))
  );
}

function receiverRole(receiver) {
  return receiver?.role || receiver?.receiver_role || "receiver";
}

function receiverParentId(receiver) {
  return receiver?.parent_id || receiver?.parent || "";
}

function receiverIcon(receiver) {
  const role = receiverRole(receiver);

  if (role === "esm") return "▣";
  if (role === "erc") return "▤";
  if (role === "device") return "◈";

  return "▸";
}

function receiverRoleLabel(receiver) {
  const role = receiverRole(receiver);

  if (role === "esm") return "ESM";
  if (role === "erc") return "ERC";
  if (role === "device") return "DEV";

  return "LOG";
}

function setReceiverMessage(message, type = "info") {
  if (!receiverMessage) return;

  receiverMessage.textContent = message || "";
  receiverMessage.className = `rule-message ${type}`;
}

function selectedReceiver() {
  return allReceivers.find((receiver) => receiver.id === selectedReceiverId) || null;
}

function policyGroupName(id) {
  const group = allPolicyGroups.find((item) => item.id === id);
  return group ? (group.name || group.id) : id;
}

function buildReceiverTree() {
  const byParent = new Map();

  for (const receiver of allReceivers) {
    const parent = receiverParentId(receiver) || "__root__";

    if (!byParent.has(parent)) {
      byParent.set(parent, []);
    }

    byParent.get(parent).push(receiver);
  }

  for (const list of byParent.values()) {
    list.sort((a, b) => {
      const roleOrder = {
        esm: 1,
        erc: 2,
        receiver: 3,
        device: 4
      };

      const ar = roleOrder[receiverRole(a)] || 99;
      const br = roleOrder[receiverRole(b)] || 99;

      if (ar !== br) return ar - br;

      return String(a.name || a.id).localeCompare(String(b.name || b.id));
    });
  }

  return byParent;
}

function receiverMatchesSearch(receiver) {
  const q = receiverTreeSearch.trim().toLowerCase();

  if (!q) return true;

  const haystack = [
    receiver.id,
    receiver.name,
    receiver.path,
    receiver.source_type,
    receiverRole(receiver)
  ].join(" ").toLowerCase();

  return haystack.includes(q);
}

function receiverHasVisibleDescendant(receiver, byParent) {
  const children = byParent.get(receiver.id) || [];

  for (const child of children) {
    if (receiverMatchesSearch(child)) return true;
    if (receiverHasVisibleDescendant(child, byParent)) return true;
  }

  return false;
}

function renderReceiverTreeNode(receiver, byParent, depth = 0) {
  const children = byParent.get(receiver.id) || [];
  const hasChildren = children.length > 0;
  const expanded = receiverTreeExpanded.has(receiver.id);
  const selected = selectedReceiverId === receiver.id;
  const enabled = receiver.enabled !== false;

  const visibleSelf = receiverMatchesSearch(receiver);
  const visibleChild = receiverHasVisibleDescendant(receiver, byParent);

  if (!visibleSelf && !visibleChild) {
    return "";
  }

  const indent = depth * 16;
  const childHtml = children
    .map((child) => renderReceiverTreeNode(child, byParent, depth + 1))
    .join("");

  return `
    <div class="receiver-tree-node">
      <div
        class="receiver-tree-row ${selected ? "selected" : ""} ${enabled ? "" : "disabled"}"
        style="padding-left: ${indent}px"
        data-receiver-id="${escapeAttr(receiver.id)}"
      >
        ${
          hasChildren
            ? `<button
                 class="receiver-tree-caret"
                 type="button"
                 data-toggle-receiver-id="${escapeAttr(receiver.id)}"
               >${expanded ? "▾" : "▸"}</button>`
            : `<span class="receiver-tree-spacer"></span>`
        }

        <div class="receiver-tree-node-main">
          <span class="receiver-tree-icon">${escapeHtml(receiverIcon(receiver))}</span>
          <span class="receiver-tree-label" title="${escapeAttr(receiver.name || receiver.id)}">
            ${escapeHtml(receiver.name || receiver.id)}
          </span>
          <span class="receiver-tree-role">${escapeHtml(receiverRoleLabel(receiver))}</span>
        </div>
      </div>

      ${
        hasChildren
          ? `<div class="receiver-tree-children ${expanded ? "" : "collapsed"}">${childHtml}</div>`
          : ""
      }
    </div>
  `;
}

function renderReceivers() {
  if (!receiversList || !receiversCount) return;

  receiversCount.textContent = String(allReceivers.length);

  if (!allReceivers.length) {
    receiversList.innerHTML = `<div class="empty">No receivers configured</div>`;
    renderSelectedReceiverWorkspace();
    return;
  }

  const byParent = buildReceiverTree();
  let roots = byParent.get("__root__") || [];

  if (!roots.length) {
    roots = allReceivers.filter((receiver) => !receiverParentId(receiver));
  }

  receiversList.innerHTML = roots
    .map((receiver) => renderReceiverTreeNode(receiver, byParent, 0))
    .join("");

  receiversList.querySelectorAll("[data-toggle-receiver-id]").forEach((btn) => {
    btn.addEventListener("click", (event) => {
      event.stopPropagation();

      const id = btn.dataset.toggleReceiverId;

      if (receiverTreeExpanded.has(id)) {
        receiverTreeExpanded.delete(id);
      } else {
        receiverTreeExpanded.add(id);
      }

      saveReceiverTreeExpanded();
      renderReceivers();
    });
  });

  receiversList.querySelectorAll("[data-receiver-id]").forEach((row) => {
    row.addEventListener("click", () => {
      selectReceiver(row.dataset.receiverId);
    });

    row.addEventListener("dblclick", () => {
      openReceiverSettings(row.dataset.receiverId);
    });
  });

  renderSelectedReceiverWorkspace();
}

function selectReceiver(id) {
  const receiver = allReceivers.find((item) => item.id === id);

  if (!receiver) {
    selectedReceiverId = "";
    renderSelectedReceiverWorkspace();
    renderReceivers();
    return;
  }

  selectedReceiverId = receiver.id;

  if (receiverParentId(receiver)) {
    receiverTreeExpanded.add(receiverParentId(receiver));
    saveReceiverTreeExpanded();
  }

  renderReceivers();
  renderSelectedReceiverWorkspace();
  setReceiverMessage(`Selected receiver: ${receiver.name || receiver.id}`, "info");
}

function renderSelectedReceiverWorkspace() {
  const receiver = selectedReceiver();

  if (!receiver) {
    if (receiverEmptyState) receiverEmptyState.classList.remove("hidden");
    if (receiverDetailPanel) receiverDetailPanel.classList.add("hidden");
    return;
  }

  if (receiverEmptyState) receiverEmptyState.classList.add("hidden");
  if (receiverDetailPanel) receiverDetailPanel.classList.remove("hidden");

  if (receiverDetailTitle) {
    receiverDetailTitle.textContent = receiver.name || receiver.id;
  }

  if (receiverDetailSubtitle) {
    receiverDetailSubtitle.textContent = [
      receiver.id,
      receiverRoleLabel(receiver),
      receiver.source_type || "unknown source",
      receiver.path || ""
    ].filter(Boolean).join(" · ");
  }

  renderReceiverDashboard();
  renderReceiverCustomDashboards();
  renderReceiverParserRules();
}

function valueText(value) {
  if (value === null || value === undefined) return "";
  if (typeof value === "string") return value;
  if (typeof value === "number" || typeof value === "boolean") return String(value);

  try {
    return JSON.stringify(value);
  } catch (_) {
    return String(value);
  }
}

function objectToSearchText(obj) {
  if (!obj || typeof obj !== "object") return "";

  const parts = [];

  for (const [key, value] of Object.entries(obj)) {
    parts.push(key);
    parts.push(valueText(value));
  }

  return parts.join(" ").toLowerCase();
}

function getEventFieldValue(item, field) {
  const event = item.event || {};

  if (!field) return "";

  if (item[field] !== undefined && item[field] !== null) {
    return valueText(item[field]);
  }

  if (event[field] !== undefined && event[field] !== null) {
    return valueText(event[field]);
  }

  if (event.fields && typeof event.fields === "object" && event.fields[field] !== undefined) {
    return valueText(event.fields[field]);
  }

  if (event.extracted && typeof event.extracted === "object" && event.extracted[field] !== undefined) {
    return valueText(event.extracted[field]);
  }

  const aliasMap = {
    "source.ip": "src_ip",
    "source.port": "src_port",
    "destination.ip": "dst_ip",
    "destination.port": "dst_port",
    "user.name": "user",
    "actor.name": "user",
    "account.name": "user",
    "process.name": "process_name",
    "process.command_line": "command",
    "network.protocol": "protocol",
    "event.name": "event_name",
    "event.type": "event_type",
    "event.category": "event_category",
    "event.action": "event_action",
    "event.outcome": "event_outcome",
    "event.severity": "severity",
    "host.name": "host",
    "service.name": "service",
    "event.error_code": "error_code",
    "linux.kernel.error_code": "error_code"
  };

  const alias = aliasMap[field];

  if (alias && event[alias] !== undefined && event[alias] !== null) {
    return valueText(event[alias]);
  }

  if (alias && item[alias] !== undefined && item[alias] !== null) {
    return valueText(item[alias]);
  }

  return "";
}

function eventSearchHaystack(item) {
  const event = item.event || {};

  return [
    item.id,
    item.event_id,
    item.event_name,
    item.event_type,
    item.source,
    item.source_type,
    item.severity,
    item.host,
    item.receiver_id,
    item.receiver_name,

    event.event_id,
    event.event_name,
    event.event_type,
    event.event_code,
    event.event_category,
    event.event_action,
    event.event_outcome,
    event.source,
    event.source_type,
    event.severity,
    event.host,
    event.receiver_id,
    event.receiver_name,
    event.parser_status,
    event.parser_rule_id,
    event.parser_rule_name,
    event.policy_group_id,
    event.raw,
    event.message,
    event.user,
    event.src_ip,
    event.src_port,
    event.dst_ip,
    event.dst_port,
    event.process_name,
    event.command,
    event.cmdline,
    event.path,
    event.program,
    event.protocol,

    objectToSearchText(event.fields),
    objectToSearchText(event.extracted),
    objectToSearchText(event)
  ].join(" ").toLowerCase();
}

function tokenizeReceiverKql(query) {
  const tokens = [];
  const re = /"([^"]*)"|'([^']*)'|(\S+)/g;
  let match;

  while ((match = re.exec(query)) !== null) {
    tokens.push(match[1] ?? match[2] ?? match[3]);
  }

  return tokens;
}

function parseKqlTerm(token) {
  const negated = token.startsWith("-");
  const clean = negated ? token.slice(1) : token;

  const colonIndex = clean.indexOf(":");

  if (colonIndex <= 0) {
    return {
      type: "text",
      negated,
      value: clean
    };
  }

  return {
    type: "field",
    negated,
    field: clean.slice(0, colonIndex),
    value: clean.slice(colonIndex + 1)
  };
}

function kqlTermMatches(item, token) {
  if (!token) return true;

  const upper = token.toUpperCase();
  if (upper === "AND" || upper === "OR") return true;

  let term = parseKqlTerm(token);

  if (upper.startsWith("NOT ")) {
    term = parseKqlTerm(token.slice(4));
    term.negated = !term.negated;
  }

  let matched = false;

  if (term.type === "text") {
    const value = String(term.value || "").toLowerCase();
    matched = !value || eventSearchHaystack(item).includes(value);
  } else {
    const expected = String(term.value || "").toLowerCase();
    const actual = getEventFieldValue(item, term.field).toLowerCase();

    if (expected === "*") {
      matched = Boolean(actual);
    } else if (expected.includes("*")) {
      const pattern = expected
        .replace(/[.+?^${}()|[\]\\]/g, "\\$&")
        .replace(/\*/g, ".*");

      matched = new RegExp(`^${pattern}$`, "i").test(actual);
    } else {
      matched = actual.includes(expected);
    }
  }

  return term.negated ? !matched : matched;
}

function receiverKqlMatches(item, query) {
  const q = String(query || "").trim();

  if (!q) return true;

  const rawTokens = tokenizeReceiverKql(q);

  if (!rawTokens.length) return true;

  let groups = [[]];

  for (const token of rawTokens) {
    if (token.toUpperCase() === "OR") {
      groups.push([]);
      continue;
    }

    if (token.toUpperCase() === "AND") {
      continue;
    }

    if (token.toUpperCase() === "NOT") {
      const nextIndex = rawTokens.indexOf(token) + 1;
      continue;
    }

    groups[groups.length - 1].push(token);
  }

  return groups.some((group) => group.every((token, index) => {
    if (token.toUpperCase() === "NOT") {
      const next = group[index + 1];
      return next ? !kqlTermMatches(item, next) : true;
    }

    if (index > 0 && group[index - 1]?.toUpperCase() === "NOT") {
      return true;
    }

    return kqlTermMatches(item, token);
  }));
}

function prettyFieldsTable(obj) {
  if (!obj || typeof obj !== "object" || !Object.keys(obj).length) {
    return `<div class="receiver-field-empty">No fields</div>`;
  }

  return `
    <div class="receiver-fields-table">
      ${Object.entries(obj).map(([key, value]) => `
        <div class="receiver-field-row">
          <div class="receiver-field-key">${escapeHtml(key)}</div>
          <div class="receiver-field-value">${escapeHtml(valueText(value))}</div>
        </div>
      `).join("")}
    </div>
  `;
}

function eventMatchesReceiverFilters(item) {
  if (currentReceiverParserStatusFilter) {
    if (eventParserStatus(item) !== currentReceiverParserStatusFilter) {
      return false;
    }
  }

  if (currentReceiverKqlFilter.trim()) {
    return receiverKqlMatches(item, currentReceiverKqlFilter);
  }

  return true;
}

function updateReceiverFilterStateFromInputs() {
  currentReceiverParserStatusFilter = receiverParserStatusFilterInput?.value || "";
  currentReceiverKqlFilter = receiverKqlFilterInput?.value || "";
}

function clearReceiverFilters() {
  currentReceiverParserStatusFilter = "";
  currentReceiverKqlFilter = "";

  if (receiverParserStatusFilterInput) receiverParserStatusFilterInput.value = "";
  if (receiverKqlFilterInput) receiverKqlFilterInput.value = "";

  renderSelectedReceiverWorkspace();
}

function receiverEvents() {
  const receiver = selectedReceiver();

  if (!receiver) return [];

  return allEvents
    .filter((item) => {
      const event = item.event || {};
      return item.receiver_id === receiver.id || event.receiver_id === receiver.id;
    })
    .filter(eventMatchesReceiverFilters);
}

function eventParserStatus(item) {
  const event = item.event || {};
  return item.parser_status || event.parser_status || "";
}

function eventTypeOf(item) {
  const event = item.event || {};
  return item.event_type || event.event_type || "unknown_event";
}

function countBy(items, getter) {
  const out = {};

  for (const item of items) {
    const key = getter(item) || "unknown";
    out[key] = (out[key] || 0) + 1;
  }

  return out;
}

function renderReceiverMiniBars(container, counts) {
  if (!container) return;

  const entries = Object.entries(counts || {})
    .sort((a, b) => b[1] - a[1])
    .slice(0, 12);

  if (!entries.length) {
    container.innerHTML = `<div class="empty small">No data for selected receiver</div>`;
    return;
  }

  const max = Math.max(...entries.map(([, count]) => count), 1);

  container.innerHTML = entries.map(([label, count]) => `
    <div class="bar-row">
      <div class="bar-label">${escapeHtml(label)}</div>
      <div class="bar-track">
        <div class="bar-fill" style="width:${Math.max(4, Math.round((count / max) * 100))}%"></div>
      </div>
      <div class="bar-value">${escapeHtml(String(count))}</div>
    </div>
  `).join("");
}

function renderReceiverRecentEvents(events) {
  if (!receiverRecentEvents) return;

  const recent = events.slice(0, 25);

  if (!recent.length) {
    receiverRecentEvents.innerHTML = `<div class="empty">No events match selected receiver filters</div>`;
    return;
  }

  receiverRecentEvents.innerHTML = `
    <div class="receiver-log-table">
      <div class="receiver-log-table-head">
        <span>Time</span>
        <span>Event</span>
        <span>Status</span>
        <span>Severity</span>
        <span>Source</span>
      </div>

      ${recent.map((item) => {
        const event = item.event || {};
        const eventName = item.event_name || event.event_name || eventTypeOf(item);
        const ts = item.received_at || event.received_at || item.ts || event.ts || "";
        const status = eventParserStatus(item) || "unknown";
        const severity = item.severity || event.severity || "info";
        const source = item.source || event.source || "";
        const raw = event.raw || item.raw || "";

        const fields = event.fields || {};
        const extracted = event.extracted || {};

        const chipFields = [
          event.user ? ["user", event.user] : null,
          event.src_ip ? ["src_ip", event.src_ip] : null,
          event["source.ip"] ? ["source.ip", event["source.ip"]] : null,
          event.process_name ? ["process", event.process_name] : null,
          event.parser_rule_name ? ["parser", event.parser_rule_name] : null
        ].filter(Boolean);

        return `
          <details class="receiver-log-row">
            <summary>
              <span class="receiver-log-time">${escapeHtml(ts)}</span>
              <span class="receiver-log-event">${escapeHtml(eventName)}</span>
              <span class="receiver-log-status ${escapeAttr(status)}">${escapeHtml(status)}</span>
              <span class="severity ${escapeAttr(severity)}">${escapeHtml(severity)}</span>
              <span class="receiver-log-source">${escapeHtml(source)}</span>
            </summary>

            <div class="receiver-log-body">
              ${
                chipFields.length
                  ? `<div class="receiver-log-summary-line">
                      ${chipFields.map(([key, value]) => `
                        <span class="receiver-log-chip">
                          ${escapeHtml(key)}: <strong>${escapeHtml(valueText(value))}</strong>
                        </span>
                      `).join("")}
                    </div>`
                  : ""
              }

              <div>
                <div class="receiver-log-raw-title">Raw log</div>
                <pre class="receiver-log-raw">${escapeHtml(raw || "No raw log available")}</pre>
              </div>

              <div class="receiver-log-details-grid">
                <div>
                  <div class="receiver-log-raw-title">Mapped SIEM fields</div>
                  ${prettyFieldsTable(fields)}
                </div>

                <div>
                  <div class="receiver-log-raw-title">Extracted variables</div>
                  ${prettyFieldsTable(extracted)}
                </div>
              </div>

              <details class="receiver-full-json-details">
                <summary>Show full event JSON</summary>
                <pre class="receiver-log-json">${escapeHtml(prettyJson(event))}</pre>
              </details>
            </div>
          </details>
        `;
      }).join("")}
    </div>
  `;
}

function enabledParserRulesForReceiver(receiverId) {
  return allParserRules.filter((rule) => {
    const ids = Array.isArray(rule.enabled_receivers) ? rule.enabled_receivers : [];
    return ids.includes(receiverId);
  });
}

function renderReceiverDashboard() {
  const receiver = selectedReceiver();

  if (!receiver) return;

  const events = receiverEvents();
  const parsed = events.filter((item) => eventParserStatus(item) === "parsed");
  const unknown = events.filter((item) => eventParserStatus(item) === "unparsed");
  const parserRules = enabledParserRulesForReceiver(receiver.id);

  if (receiverKpiEvents) receiverKpiEvents.textContent = String(events.length);
  if (receiverKpiUnknown) receiverKpiUnknown.textContent = String(unknown.length);
  if (receiverKpiParsed) receiverKpiParsed.textContent = String(parsed.length);
  if (receiverKpiParserRules) receiverKpiParserRules.textContent = String(parserRules.length);

  renderReceiverMiniBars(receiverEventTypesWidget, countBy(events, eventTypeOf));
  renderReceiverMiniBars(receiverParserStatusWidget, countBy(events, eventParserStatus));
  renderReceiverRecentEvents(events);
}

function renderReceiverCustomDashboards() {
  if (!receiverCustomDashboardsList) return;

  const receiver = selectedReceiver();

  if (!receiver) {
    receiverCustomDashboardsList.innerHTML = `<div class="empty small">Select receiver first</div>`;
    return;
  }

  const selected = new Set(Array.isArray(receiver.dashboard_ids) ? receiver.dashboard_ids : []);

  if (!customDashboards.length) {
    receiverCustomDashboardsList.innerHTML = `<div class="empty small">No custom dashboards configured</div>`;
    return;
  }

  receiverCustomDashboardsList.innerHTML = customDashboards.map((dashboard) => `
    <label class="receiver-check-item">
      <input
        type="checkbox"
        value="${escapeAttr(dashboard.id)}"
        ${selected.has(dashboard.id) ? "checked" : ""}
      />
      <span>
        <strong>${escapeHtml(dashboard.name || dashboard.id)}</strong>
        <em>${escapeHtml(dashboard.description || dashboard.id)}</em>
      </span>
    </label>
  `).join("");
}

function selectedReceiverDashboardIds() {
  if (!receiverCustomDashboardsList) return [];

  return Array.from(receiverCustomDashboardsList.querySelectorAll("input[type='checkbox']:checked"))
    .map((input) => input.value)
    .filter(Boolean);
}

function activateReceiverDashboardTab(tab) {
  currentReceiverDashboardTab = tab || "standard";

  document.querySelectorAll(".receiver-dashboard-tab").forEach((btn) => {
    btn.classList.toggle("active", btn.dataset.receiverDashboardTab === currentReceiverDashboardTab);
  });

const views = {
  standard: receiverDashboardStandard,
  custom: receiverDashboardCustom
};

  Object.entries(views).forEach(([key, view]) => {
    if (!view) return;
    view.classList.toggle("active", key === currentReceiverDashboardTab);
  });
}

function renderReceiverPolicyGroupChecks(selectedIds = []) {
  if (!receiverPolicyGroupsList) return;

  const selected = new Set(selectedIds || []);

  if (!allPolicyGroups.length) {
    receiverPolicyGroupsList.innerHTML = `<div class="empty small">No policy groups configured</div>`;
    return;
  }

  receiverPolicyGroupsList.innerHTML = allPolicyGroups.map((group) => `
    <label class="receiver-check-item">
      <input
        type="checkbox"
        value="${escapeAttr(group.id)}"
        ${selected.has(group.id) ? "checked" : ""}
      />
      <span>
        <strong>${escapeHtml(group.name || group.id)}</strong>
        <em>${escapeHtml(group.description || group.id)}</em>
      </span>
    </label>
  `).join("");
}

function selectedReceiverPolicyGroupIds() {
  if (!receiverPolicyGroupsList) return [];

  return Array.from(receiverPolicyGroupsList.querySelectorAll("input[type='checkbox']:checked"))
    .map((input) => input.value)
    .filter(Boolean);
}

function renderReceiverParentOptions(currentId = "", selectedParentId = "") {
  if (!receiverParentInput) return;

  const candidates = allReceivers.filter((receiver) => receiver.id !== currentId);

  receiverParentInput.innerHTML = `
    <option value="">No parent / root</option>
    ${candidates.map((receiver) => `
      <option value="${escapeAttr(receiver.id)}" ${receiver.id === selectedParentId ? "selected" : ""}>
        ${escapeHtml(receiver.name || receiver.id)} (${escapeHtml(receiverRoleLabel(receiver))})
      </option>
    `).join("")}
  `;
}

function activateReceiverSettingsTab(tab) {
  currentReceiverSettingsTab = tab || "config";

  document.querySelectorAll(".receiver-settings-tab").forEach((btn) => {
    btn.classList.toggle("active", btn.dataset.receiverSettingsTab === currentReceiverSettingsTab);
  });

const views = {
  config: $("receiverSettingsConfig"),
  parsers: $("receiverSettingsParsers"),
  newParser: $("receiverSettingsNewParser")
};

  Object.entries(views).forEach(([key, view]) => {
    if (!view) return;
    view.classList.toggle("active", key === currentReceiverSettingsTab);
  });
}

function receiverFromForm() {
  const id = receiverIdInput.value.trim();

  if (!id) throw new Error("Receiver ID is required");
  if (!receiverNameInput.value.trim()) throw new Error("Receiver name is required");

  const role = receiverRoleInput?.value || "receiver";
  const type = receiverTypeInput?.value || "file";
  const path = receiverPathInput?.value.trim() || "";

  if ((role === "receiver" || role === "device") && !path) {
    throw new Error("Receiver path is required for log receivers/devices");
  }

return {
  id,
  name: receiverNameInput.value.trim(),
  role,
  parent_id: receiverParentInput?.value || "",
  type,
  path,
  enabled: receiverEnabledInput.checked,
  source_type: receiverSourceTypeInput.value.trim() || "linux_log",
  initial_position: receiverInitialPositionInput.value || "end",

  file_pattern: receiverFilePatternInput?.value.trim() || "*.log",
  recursive: receiverRecursiveInput?.checked || false,

  policy_group_ids: selectedReceiverPolicyGroupIds(),
  dashboard_ids: selectedReceiver()?.dashboard_ids || []
};
}

function fillReceiverForm(receiver) {
  editingReceiverId = receiver?.id || "";

  receiverIdInput.disabled = Boolean(editingReceiverId);
  receiverIdInput.value = receiver?.id || "";
  receiverEnabledInput.checked = receiver?.enabled !== false;
  receiverNameInput.value = receiver?.name || "";

  if (receiverRoleInput) receiverRoleInput.value = receiverRole(receiver);
  renderReceiverParentOptions(receiver?.id || "", receiverParentId(receiver));

receiverTypeInput.value = receiver?.type || "file";
receiverPathInput.value = receiver?.path || "";

if (receiverFilePatternInput) {
  receiverFilePatternInput.value = receiver?.file_pattern || "*.log";
}

if (receiverRecursiveInput) {
  receiverRecursiveInput.checked = receiver?.recursive === true;
}

receiverSourceTypeInput.value = receiver?.source_type || "linux_log";
receiverInitialPositionInput.value = receiver?.initial_position || "end";

  renderReceiverPolicyGroupChecks(receiver?.policy_group_ids || []);

  if (deleteReceiverBtn) {
    deleteReceiverBtn.disabled = !editingReceiverId;
  }
}

function newReceiver() {
  const parent = selectedReceiver();

  selectedReceiverId = parent?.id || selectedReceiverId;
  editingReceiverId = "";

  if (receiverSettingsPanel) {
    receiverSettingsPanel.classList.remove("hidden");
  }

  receiverSettingsTitle.textContent = "New receiver";
  receiverSettingsSubtitle.textContent = parent
    ? `Parent: ${parent.name || parent.id}`
    : "Create a new ESM, ERC, receiver or child source.";

fillReceiverForm({
  id: "",
  name: "",
  role: parent ? "receiver" : "esm",
  parent_id: parent?.id || "",
  type: "file",
  path: "",
  enabled: true,
  source_type: "linux_log",
  initial_position: "end",
  file_pattern: "*.log",
  recursive: false,
  policy_group_ids: []
});

  clearParserRuleFormForReceiver();
  activateReceiverSettingsTab("config");

  setReceiverMessage("Creating new receiver", "info");
}

function openReceiverSettings(id = selectedReceiverId) {
  const receiver = allReceivers.find((item) => item.id === id);

  if (!receiver) {
    setReceiverMessage("Select receiver first.", "error");
    return;
  }

  selectedReceiverId = receiver.id;
  editingReceiverId = receiver.id;

  if (receiverSettingsPanel) {
    receiverSettingsPanel.classList.remove("hidden");
  }

  receiverSettingsTitle.textContent = receiver.name || receiver.id;
  receiverSettingsSubtitle.textContent = `${receiver.path || "root node"} · ${receiver.source_type || receiverRoleLabel(receiver)}`;

  fillReceiverForm(receiver);
  clearParserRuleFormForReceiver();
  renderReceiverParserRules();
  renderReceivers();
  renderSelectedReceiverWorkspace();
  activateReceiverSettingsTab("config");

  setReceiverMessage(`Opened receiver settings: ${receiver.name || receiver.id}`, "info");
}

function closeReceiverSettings() {
  editingReceiverId = "";

  if (receiverSettingsPanel) {
    receiverSettingsPanel.classList.add("hidden");
  }

  setReceiverMessage("", "info");
}

async function saveReceiver() {
  if (!canAdmin()) {
    setReceiverMessage("Only admin can save receivers.", "error");
    return;
  }

  try {
    const receiver = receiverFromForm();
    const exists = allReceivers.some((item) => item.id === receiver.id);

    const method = exists ? "PUT" : "POST";
    const url = exists
      ? `/api/receivers/${encodeURIComponent(receiver.id)}`
      : "/api/receivers";

    await fetchJson(url, {
      method,
      headers: {
        "Content-Type": "application/json"
      },
      body: JSON.stringify(receiver)
    });

    setReceiverMessage(`${exists ? "Updated" : "Created"} receiver: ${receiver.id}`, "ok");

    selectedReceiverId = receiver.id;
    editingReceiverId = receiver.id;

    if (receiver.parent_id) {
      receiverTreeExpanded.add(receiver.parent_id);
      saveReceiverTreeExpanded();
    }

    await loadAll();

    openReceiverSettings(receiver.id);
    activateTab("receivers");
  } catch (err) {
    setReceiverMessage(`Save receiver failed: ${err.message}`, "error");
  }
}

async function deleteReceiver(id = editingReceiverId || selectedReceiverId) {
  if (!canAdmin()) {
    setReceiverMessage("Only admin can delete receivers.", "error");
    return;
  }

  const receiver = allReceivers.find((item) => item.id === id);

  if (!receiver) {
    setReceiverMessage("No receiver selected.", "error");
    return;
  }

  const childCount = allReceivers.filter((item) => receiverParentId(item) === receiver.id).length;

  if (childCount > 0) {
    setReceiverMessage(`Cannot delete ${receiver.id}: it has ${childCount} child receiver(s). Delete or move children first.`, "error");
    return;
  }

  if (!confirm(`Delete receiver ${receiver.id}?`)) {
    return;
  }

  try {
    await fetchJson(`/api/receivers/${encodeURIComponent(receiver.id)}`, {
      method: "DELETE"
    });

    setReceiverMessage(`Deleted receiver: ${receiver.id}`, "ok");

    selectedReceiverId = "";
    editingReceiverId = "";

    closeReceiverSettings();
    await loadAll();
    activateTab("receivers");
  } catch (err) {
    setReceiverMessage(`Delete receiver failed: ${err.message}`, "error");
  }
}

async function saveReceiverDashboards() {
  if (!canAdmin()) {
    setReceiverMessage("Only admin can assign dashboards.", "error");
    return;
  }

  const receiver = selectedReceiver();

  if (!receiver) {
    setReceiverMessage("Select receiver first.", "error");
    return;
  }

  const nextReceiver = {
    ...receiver,
    dashboard_ids: selectedReceiverDashboardIds()
  };

  try {
    await fetchJson(`/api/receivers/${encodeURIComponent(receiver.id)}`, {
      method: "PUT",
      headers: {
        "Content-Type": "application/json"
      },
      body: JSON.stringify(nextReceiver)
    });

    setReceiverMessage(`Saved dashboard assignment for ${receiver.id}`, "ok");

    await loadAll();
    selectedReceiverId = receiver.id;
    renderSelectedReceiverWorkspace();
    activateReceiverDashboardTab("custom");
  } catch (err) {
    setReceiverMessage(`Save dashboards failed: ${err.message}`, "error");
  }
}

function renderReceiverParserRules() {
  if (!receiverParserRulesList) return;

  const receiver = selectedReceiver();

  if (!receiver) {
    receiverParserRulesList.innerHTML = `<div class="empty">Select a receiver to view parser rules.</div>`;
    selectedParserRuleId = "";
    return;
  }

  if (!allParserRules.length) {
    receiverParserRulesList.innerHTML = `<div class="empty">No parser rules configured.</div>`;
    selectedParserRuleId = "";
    return;
  }

  const stillExists = allParserRules.some((rule) => rule.id === selectedParserRuleId);
  if (!stillExists) {
    selectedParserRuleId = "";
  }

  receiverParserRulesList.innerHTML = allParserRules.map((rule) => {
    const enabledReceivers = Array.isArray(rule.enabled_receivers) ? rule.enabled_receivers : [];
    const enabledHere = enabledReceivers.includes(receiver.id);
    const globallyEnabled = rule.enabled !== false;
    const selected = selectedParserRuleId === rule.id;

    return `
      <article
        class="receiver-parser-rule-row ${selected ? "selected" : ""} ${enabledHere && globallyEnabled ? "" : "disabled"}"
        data-parser-rule-id="${escapeAttr(rule.id)}"
      >
        <div class="receiver-parser-rule-top">
          <div>
            <strong>${escapeHtml(rule.name || rule.id)}</strong>
            <span>${escapeHtml(rule.id || "")}</span>
          </div>

          <span class="rule-state ${enabledHere && globallyEnabled ? "enabled" : "disabled"}">
            ${enabledHere && globallyEnabled ? "enabled here" : "disabled here"}
          </span>
        </div>

        <div class="receiver-parser-rule-meta">
          <span class="badge">${escapeHtml(rule.policy_group_id || "no policy group")}</span>
          <span class="badge">${escapeHtml(rule.match_type || "regex")}</span>
          <span class="badge">${escapeHtml(rule.constants?.event_type || "no event type")}</span>
          <span class="badge">receivers: ${escapeHtml((rule.enabled_receivers || []).join(", ") || "none")}</span>
        </div>
      </article>
    `;
  }).join("");

  receiverParserRulesList.querySelectorAll("[data-parser-rule-id]").forEach((row) => {
    row.addEventListener("click", () => {
      selectedParserRuleId = row.dataset.parserRuleId;
      renderReceiverParserRules();
    });

    row.addEventListener("dblclick", () => {
      selectedParserRuleId = row.dataset.parserRuleId;
      editSelectedParserRuleFromSettings();
    });
  });
}

async function toggleParserRuleForSelectedReceiver(ruleId) {
  if (!canAdmin()) {
    setReceiverMessage("Only admin can change parser rollout.", "error");
    return;
  }

  const receiver = selectedReceiver();
  if (!receiver) {
    setReceiverMessage("Select a receiver first.", "error");
    return;
  }

  const rule = allParserRules.find((item) => item.id === ruleId);
  if (!rule) {
    setReceiverMessage(`Parser rule not found: ${ruleId}`, "error");
    return;
  }

  const nextRule = cloneJson(rule);
  const ids = new Set(Array.isArray(nextRule.enabled_receivers) ? nextRule.enabled_receivers : []);

  if (ids.has(receiver.id)) {
    ids.delete(receiver.id);
  } else {
    ids.add(receiver.id);
  }

  nextRule.enabled_receivers = Array.from(ids);

  try {
    await fetchJson(`/api/parser-rules/${encodeURIComponent(ruleId)}`, {
      method: "PUT",
      headers: {
        "Content-Type": "application/json"
      },
      body: JSON.stringify(nextRule)
    });

    setReceiverMessage(`Updated parser rollout: ${ruleId}`, "ok");

    await loadAll();
    selectedReceiverId = receiver.id;
    renderReceiverParserRules();
    renderReceivers();
    activateTab("receivers");
  } catch (err) {
    setReceiverMessage(`Parser rollout update failed: ${err.message}`, "error");
  }
}

function newParserRuleFromSettings() {
  const receiver = selectedReceiver();

  if (!receiver) {
    setReceiverMessage("Select receiver first.", "error");
    return;
  }

  selectedParserRuleId = "";
  clearParserRuleFormForReceiver();
  activateReceiverSettingsTab("newParser");
  setReceiverMessage("Creating new parser rule.", "info");
}

function editSelectedParserRuleFromSettings() {
  if (!selectedParserRuleId) {
    setReceiverMessage("Select parser rule first.", "error");
    return;
  }

  editParserRuleForReceiver(selectedParserRuleId);
}

async function deleteSelectedParserRuleFromSettings() {
  if (!selectedParserRuleId) {
    setReceiverMessage("Select parser rule first.", "error");
    return;
  }

  await deleteParserRuleForReceiver(selectedParserRuleId);
  selectedParserRuleId = "";
  renderReceiverParserRules();
}

async function toggleSelectedParserRuleFromSettings() {
  if (!canAdmin()) {
    setReceiverMessage("Only admin can change parser rollout.", "error");
    return;
  }

  const receiver = selectedReceiver();

  if (!receiver) {
    setReceiverMessage("Select receiver first.", "error");
    return;
  }

  if (!selectedParserRuleId) {
    setReceiverMessage("Select parser rule first.", "error");
    return;
  }

  const rule = allParserRules.find((item) => item.id === selectedParserRuleId);

  if (!rule) {
    setReceiverMessage(`Parser rule not found: ${selectedParserRuleId}`, "error");
    return;
  }

  const nextRule = cloneJson(rule);
  const enabledReceivers = new Set(
    Array.isArray(nextRule.enabled_receivers) ? nextRule.enabled_receivers : []
  );

  if (enabledReceivers.has(receiver.id)) {
    enabledReceivers.delete(receiver.id);
  } else {
    enabledReceivers.add(receiver.id);
  }

  nextRule.enabled_receivers = Array.from(enabledReceivers);

  try {
    await fetchJson(`/api/parser-rules/${encodeURIComponent(rule.id)}`, {
      method: "PUT",
      headers: {
        "Content-Type": "application/json"
      },
      body: JSON.stringify(nextRule)
    });

    setReceiverMessage(
      `${enabledReceivers.has(receiver.id) ? "Enabled" : "Disabled"} parser rule on this receiver: ${rule.id}`,
      "ok"
    );

    await loadAll();

    selectedReceiverId = receiver.id;
    selectedParserRuleId = rule.id;

    renderReceivers();
    renderSelectedReceiverWorkspace();
    renderReceiverParserRules();
    activateReceiverSettingsTab("parsers");
  } catch (err) {
    setReceiverMessage(`Parser rollout update failed: ${err.message}`, "error");
  }
}

function editParserRuleForReceiver(ruleId) {
  const rule = allParserRules.find((item) => item.id === ruleId);

  if (!rule) {
    setReceiverMessage(`Parser rule not found: ${ruleId}`, "error");
    return;
  }

  selectedParserRuleId = ruleId;

  fillParserRuleFormForReceiver(rule);
  activateReceiverSettingsTab("newParser");
}

async function deleteParserRuleForReceiver(ruleId) {
  if (!canAdmin()) {
    setReceiverMessage("Only admin can delete parser rules.", "error");
    return;
  }

  if (!confirm(`Delete parser rule ${ruleId}?`)) {
    return;
  }

  try {
    await fetchJson(`/api/parser-rules/${encodeURIComponent(ruleId)}`, {
      method: "DELETE"
    });

    setReceiverMessage(`Deleted parser rule: ${ruleId}`, "ok");

    if (selectedParserRuleId === ruleId) {
      selectedParserRuleId = "";
    }

    await loadAll();

    const receiver = selectedReceiver();
    if (receiver) {
      openReceiverSettings(receiver.id);
      activateReceiverSettingsTab("parsers");
    }
  } catch (err) {
    setReceiverMessage(`Delete parser rule failed: ${err.message}`, "error");
  }
}

function flattenFieldCatalog() {
  const out = [];

  for (const group of fieldCatalog || []) {
    const groupName = String(group.group || "Fields");
    const fields = Array.isArray(group.fields) ? group.fields : [];

    for (const field of fields) {
      out.push({
        group: groupName,
        field: String(field)
      });
    }
  }

  return out;
}

function fieldOptionsHtml(selected = "") {
  const flat = flattenFieldCatalog();

  if (!flat.length) {
    return `<option value="${escapeAttr(selected)}">${escapeHtml(selected || "No field catalog loaded")}</option>`;
  }

  const groups = {};

  for (const item of flat) {
    if (!groups[item.group]) groups[item.group] = [];
    groups[item.group].push(item.field);
  }

  return Object.entries(groups).map(([group, fields]) => `
    <optgroup label="${escapeAttr(group)}">
      ${fields.map((field) => `
        <option value="${escapeAttr(field)}" ${field === selected ? "selected" : ""}>
          ${escapeHtml(field)}
        </option>
      `).join("")}
    </optgroup>
  `).join("");
}

function guessTargetField(variable) {
  const v = lower(variable);

  if (v === "user" || v === "username" || v === "account") return "user.name";
  if (v === "actor") return "actor.name";

  if (v === "src_ip" || v === "source_ip" || v === "ip" || v === "remote_ip") return "source.ip";
  if (v === "src_port" || v === "source_port" || v === "port" || v === "remote_port") return "source.port";

  if (v === "dst_ip" || v === "destination_ip" || v === "dest_ip") return "destination.ip";
  if (v === "dst_port" || v === "destination_port" || v === "dest_port") return "destination.port";

  if (v === "protocol") return "network.protocol";
  if (v === "transport") return "network.transport";
  if (v === "direction") return "network.direction";

  if (v === "process" || v === "program" || v === "process_name" || v === "comm") return "process.name";
  if (v === "pid") return "process.pid";
  if (v === "ppid") return "process.ppid";
  if (v === "command" || v === "cmdline" || v === "command_line") return "process.command_line";

  if (v === "path" || v === "file_path" || v === "name") return "file.path";
  if (v === "target_path") return "file.target_path";

  if (v === "service") return "service.name";
  if (v === "syslog_pid") return "syslog.pid";
  if (v === "identifier" || v === "syslog_identifier") return "syslog.identifier";

  if (v === "error_code") return "event.error_code";
  if (v === "message") return "event.message";
  if (v === "reason") return "event.reason";

  if (v === "url") return "url.original";
  if (v === "domain") return "dns.question.name";

  return "custom.string1";
}

function renderParserPolicyOptionsForReceiver() {
  if (!parserPolicyGroupInput) return;

  if (!allPolicyGroups.length) {
    parserPolicyGroupInput.innerHTML = `<option value="">No policy groups</option>`;
    return;
  }

  parserPolicyGroupInput.innerHTML = allPolicyGroups.map((group) => `
    <option value="${escapeAttr(group.id)}">
      ${escapeHtml(group.name || group.id)}
    </option>
  `).join("");
}

function renderParserReceiversForReceiver(selectedIds = []) {
  if (!parserReceiversList) return;

  const selected = new Set(selectedIds || []);

  if (!allReceivers.length) {
    parserReceiversList.innerHTML = `<div class="empty small">No receivers configured</div>`;
    return;
  }

  parserReceiversList.innerHTML = allReceivers.map((receiver) => `
    <label class="receiver-check-item">
      <input
        type="checkbox"
        value="${escapeAttr(receiver.id)}"
        ${selected.has(receiver.id) ? "checked" : ""}
      />
      <span>
        <strong>${escapeHtml(receiver.name || receiver.id)}</strong>
        <em>${escapeHtml(receiver.path || receiver.id)}</em>
      </span>
    </label>
  `).join("");
}

function selectedParserReceiverIdsForReceiver() {
  if (!parserReceiversList) return [];

  return Array.from(parserReceiversList.querySelectorAll("input[type='checkbox']:checked"))
    .map((input) => input.value)
    .filter(Boolean);
}

function parserFieldOrderForReceiver() {
  return splitCsv(parserFieldOrderInput?.value || "");
}

function parserRuleFromFormForReceiver() {
  const id = parserRuleIdInput.value.trim();

  if (!id) throw new Error("Parser Rule ID is required");
  if (!parserRuleNameInput.value.trim()) throw new Error("Parser rule name is required");
  if (!parserPolicyGroupInput.value) throw new Error("Policy group is required");

  const matchType = parserMatchTypeInput.value || "regex";
  const fieldOrder = parserFieldOrderForReceiver();

  if (matchType === "regex") {
    if (!parserPatternInput.value.trim()) {
      throw new Error("Regex pattern is required");
    }

    if (!fieldOrder.length) {
      throw new Error("Field order is required for regex parser");
    }
  }

  const field_mapping = {};

  for (const [variable, targetField] of Object.entries(currentParserMapping || {})) {
    if (!variable || !targetField) continue;
    field_mapping[targetField] = `$${variable}`;
  }

  const constants = {
    event_code: parserConstantEventCodeInput.value.trim(),
    event_name: parserConstantEventNameInput.value.trim(),
    event_type: parserConstantEventTypeInput.value.trim(),
    event_category: parserConstantEventCategoryInput.value.trim(),
    event_action: parserConstantEventActionInput.value.trim(),
    event_outcome: parserConstantEventOutcomeInput.value.trim(),
    severity: parserConstantSeverityInput.value || "info",
    vendor: parserConstantVendorInput.value.trim(),
    product: parserConstantProductInput.value.trim()
  };

  const rule = {
    id,
    name: parserRuleNameInput.value.trim(),
    enabled: parserRuleEnabledInput.checked,
    policy_group_id: parserPolicyGroupInput.value,
    match_type: matchType,
    sample_raw: parserRawSampleInput.value,
    pattern: matchType === "regex" ? parserPatternInput.value : "",
    field_order: matchType === "regex" ? fieldOrder : [],
    field_mapping,
    constants,
    enabled_receivers: selectedParserReceiverIdsForReceiver()
  };

  if (matchType === "json") {
    rule.json_paths = {};
  }

  return rule;
}

function updateParserModeHintForReceiver() {
  if (!parserMatchTypeInput) return;

  const matchType = parserMatchTypeInput.value || "regex";

  if (matchType === "json") {
    if (parserPatternInput) {
      parserPatternInput.placeholder = "Not used for JSON mode";
    }

    if (parserFieldOrderInput) {
      parserFieldOrderInput.placeholder = "Not used for JSON mode";
    }
  } else {
    if (parserPatternInput) {
      parserPatternInput.placeholder = "Failed password for (invalid user )?(\\S+) from (\\S+) port (\\d+) (\\S+)";
    }

    if (parserFieldOrderInput) {
      parserFieldOrderInput.placeholder = "invalid_prefix, user, src_ip, src_port, protocol";
    }
  }
}

function updateParserRulePreviewForReceiver() {
  if (!parserRuleJsonPreview) return;

  try {
    const rule = parserRuleFromFormForReceiver();
    parserRuleJsonPreview.textContent = prettyJson(rule);
  } catch (_) {
    const draft = {
      id: parserRuleIdInput?.value || "",
      name: parserRuleNameInput?.value || "",
      enabled: parserRuleEnabledInput?.checked ?? true,
      policy_group_id: parserPolicyGroupInput?.value || "",
      match_type: parserMatchTypeInput?.value || "regex",
      sample_raw: parserRawSampleInput?.value || "",
      pattern: parserPatternInput?.value || "",
      field_order: parserFieldOrderForReceiver(),
      field_mapping: currentParserMapping || {},
      constants: {
        event_code: parserConstantEventCodeInput?.value || "",
        event_name: parserConstantEventNameInput?.value || "",
        event_type: parserConstantEventTypeInput?.value || "",
        event_category: parserConstantEventCategoryInput?.value || "",
        event_action: parserConstantEventActionInput?.value || "",
        event_outcome: parserConstantEventOutcomeInput?.value || "",
        severity: parserConstantSeverityInput?.value || "medium",
        vendor: parserConstantVendorInput?.value || "",
        product: parserConstantProductInput?.value || ""
      },
      enabled_receivers: selectedParserReceiverIdsForReceiver()
    };

    parserRuleJsonPreview.textContent = prettyJson(draft);
  }
}

function renderParserMappingForReceiver() {
  if (!parserMappingRows) return;

  const variables = Object.keys(currentParserExtracted || {});

  if (!variables.length) {
    parserMappingRows.innerHTML = `<div class="empty small">Run extraction test to build mapping rows.</div>`;
    updateParserRulePreviewForReceiver();
    return;
  }

  for (const variable of variables) {
    if (!currentParserMapping[variable]) {
      currentParserMapping[variable] = guessTargetField(variable);
    }
  }

  parserMappingRows.innerHTML = variables.map((variable) => `
    <div class="parser-mapping-row">
      <div>
        <span>Extracted variable</span>
        <strong>$${escapeHtml(variable)}</strong>
        <em>${escapeHtml(String(currentParserExtracted[variable] ?? ""))}</em>
      </div>

      <div class="field">
        <label>SIEM field</label>
        <select data-parser-variable="${escapeAttr(variable)}">
          ${fieldOptionsHtml(currentParserMapping[variable])}
        </select>
      </div>
    </div>
  `).join("");

  parserMappingRows.querySelectorAll("select[data-parser-variable]").forEach((select) => {
    select.addEventListener("change", () => {
      currentParserMapping[select.dataset.parserVariable] = select.value;
      updateParserRulePreviewForReceiver();
    });
  });

  updateParserRulePreviewForReceiver();
}

function clearParserRuleFormForReceiver() {
  editingParserRuleId = "";
  currentParserExtracted = {};
  currentParserMapping = {};

  renderParserPolicyOptionsForReceiver();

  if (parserRuleFormTitle) parserRuleFormTitle.textContent = "Create parser rule";
  if (parserEditorModePill) parserEditorModePill.textContent = "create mode";

  if (parserRuleIdInput) {
    parserRuleIdInput.disabled = false;
    parserRuleIdInput.value = "";
  }

  if (parserRuleEnabledInput) parserRuleEnabledInput.checked = true;
  if (parserRuleNameInput) parserRuleNameInput.value = "";

  const receiver = selectedReceiver();
  if (parserPolicyGroupInput) {
    const receiverGroups = Array.isArray(receiver?.policy_group_ids) ? receiver.policy_group_ids : [];
    parserPolicyGroupInput.value = receiverGroups[0] || allPolicyGroups[0]?.id || "";
  }

  if (parserMatchTypeInput) parserMatchTypeInput.value = "regex";
  updateParserModeHintForReceiver();

  if (parserRawSampleInput) parserRawSampleInput.value = "";
  if (parserPatternInput) parserPatternInput.value = "";
  if (parserFieldOrderInput) parserFieldOrderInput.value = "";

  if (parserConstantEventCodeInput) parserConstantEventCodeInput.value = "";
  if (parserConstantEventNameInput) parserConstantEventNameInput.value = "";
  if (parserConstantEventTypeInput) parserConstantEventTypeInput.value = "";
  if (parserConstantEventCategoryInput) parserConstantEventCategoryInput.value = "";
  if (parserConstantEventActionInput) parserConstantEventActionInput.value = "";
  if (parserConstantEventOutcomeInput) parserConstantEventOutcomeInput.value = "";
  if (parserConstantSeverityInput) parserConstantSeverityInput.value = "medium";
  if (parserConstantVendorInput) parserConstantVendorInput.value = "";
  if (parserConstantProductInput) parserConstantProductInput.value = "";

  const selectedReceivers = receiver?.id ? [receiver.id] : [];
  renderParserReceiversForReceiver(selectedReceivers);

  if (parserExtractedPreview) parserExtractedPreview.textContent = "No extraction test yet.";

  renderParserMappingForReceiver();

  if (createParserRuleBtn) createParserRuleBtn.disabled = false;
  if (updateParserRuleBtn) updateParserRuleBtn.disabled = true;

  updateParserRulePreviewForReceiver();
}

function fillParserRuleFormForReceiver(rule) {
  editingParserRuleId = rule.id || "";
  currentParserExtracted = {};
  currentParserMapping = {};

  renderParserPolicyOptionsForReceiver();

  if (parserRuleFormTitle) parserRuleFormTitle.textContent = "Edit parser rule";
  if (parserEditorModePill) parserEditorModePill.textContent = "edit mode";

  if (parserRuleIdInput) {
    parserRuleIdInput.disabled = true;
    parserRuleIdInput.value = rule.id || "";
  }

  if (parserRuleEnabledInput) parserRuleEnabledInput.checked = rule.enabled !== false;
  if (parserRuleNameInput) parserRuleNameInput.value = rule.name || "";
  if (parserPolicyGroupInput) parserPolicyGroupInput.value = rule.policy_group_id || "";
  if (parserMatchTypeInput) parserMatchTypeInput.value = rule.match_type || "regex";
  updateParserModeHintForReceiver();
  if (parserRawSampleInput) parserRawSampleInput.value = rule.sample_raw || "";
  if (parserPatternInput) parserPatternInput.value = rule.pattern || "";
  if (parserFieldOrderInput) parserFieldOrderInput.value = joinCsv(rule.field_order || []);

  const constants = rule.constants || {};
  if (parserConstantEventCodeInput) parserConstantEventCodeInput.value = constants.event_code || "";
  if (parserConstantEventNameInput) parserConstantEventNameInput.value = constants.event_name || "";
  if (parserConstantEventTypeInput) parserConstantEventTypeInput.value = constants.event_type || "";
  if (parserConstantEventCategoryInput) parserConstantEventCategoryInput.value = constants.event_category || "";
  if (parserConstantEventActionInput) parserConstantEventActionInput.value = constants.event_action || "";
  if (parserConstantEventOutcomeInput) parserConstantEventOutcomeInput.value = constants.event_outcome || "";
  if (parserConstantSeverityInput) parserConstantSeverityInput.value = constants.severity || "medium";
  if (parserConstantVendorInput) parserConstantVendorInput.value = constants.vendor || "";
  if (parserConstantProductInput) parserConstantProductInput.value = constants.product || "";

  const mapping = rule.field_mapping || {};
  for (const [targetField, sourceExpr] of Object.entries(mapping)) {
    const variable = String(sourceExpr || "").replace(/^\$/, "");
    if (variable) {
      currentParserMapping[variable] = targetField;
      currentParserExtracted[variable] = "";
    }
  }

  renderParserReceiversForReceiver(rule.enabled_receivers || []);
  renderParserMappingForReceiver();

  if (parserExtractedPreview) {
    parserExtractedPreview.innerHTML = detailPre(prettyJson({
      mapping: rule.field_mapping || {},
      note: "Run extraction test to preview actual extracted values."
    }));
  }

  if (createParserRuleBtn) createParserRuleBtn.disabled = true;
  if (updateParserRuleBtn) updateParserRuleBtn.disabled = false;

  updateParserRulePreviewForReceiver();
}

async function testParserRuleForReceiver() {
  if (!canAdmin()) {
    setReceiverMessage("Only admin can test parser rules.", "error");
    return;
  }

  try {
    const rule = parserRuleFromFormForReceiver();

    const data = await fetchJson("/api/parser-rules/test", {
      method: "POST",
      headers: {
        "Content-Type": "application/json"
      },
      body: JSON.stringify({
        raw: parserRawSampleInput.value,
        rule
      })
    });

    currentParserExtracted = data.extracted || {};

    if (!data.matched) {
      if (parserExtractedPreview) {
        parserExtractedPreview.innerHTML = `<div class="empty">No match. Check regex pattern and field_order.</div>`;
      }

      setReceiverMessage("Parser did not match sample.", "error");
      renderParserMappingForReceiver();
      return;
    }

    if (parserExtractedPreview) {
      parserExtractedPreview.innerHTML = detailPre(prettyJson({
        matched: data.matched,
        extracted: data.extracted,
        event_preview: data.event_preview
      }));
    }

    setReceiverMessage("Extraction matched. Map variables to SIEM fields.", "ok");
    renderParserMappingForReceiver();
  } catch (err) {
    setReceiverMessage(`Parser test failed: ${err.message}`, "error");
  }
}

async function createParserRuleForReceiver() {
  if (!canAdmin()) {
    setReceiverMessage("Only admin can create parser rules.", "error");
    return;
  }

  try {
    const rule = parserRuleFromFormForReceiver();

    await fetchJson("/api/parser-rules", {
      method: "POST",
      headers: {
        "Content-Type": "application/json"
      },
      body: JSON.stringify(rule)
    });

    setReceiverMessage(`Created parser rule: ${rule.id}`, "ok");

    await loadAll();

    const receiver = selectedReceiver();
    if (receiver) {
      openReceiverSettings(receiver.id);
      activateReceiverSettingsTab("parsers");
    }

    selectedParserRuleId = rule.id;
    renderReceiverParserRules();
    clearParserRuleFormForReceiver();

  } catch (err) {
    setReceiverMessage(`Create parser rule failed: ${err.message}`, "error");
    updateParserRulePreviewForReceiver();
  }
}

async function updateParserRuleForReceiver() {
  if (!canAdmin()) {
    setReceiverMessage("Only admin can update parser rules.", "error");
    return;
  }

  try {
    if (!editingParserRuleId) {
      throw new Error("No parser rule selected");
    }

    const rule = parserRuleFromFormForReceiver();
    rule.id = editingParserRuleId;

    await fetchJson(`/api/parser-rules/${encodeURIComponent(editingParserRuleId)}`, {
      method: "PUT",
      headers: {
        "Content-Type": "application/json"
      },
      body: JSON.stringify(rule)
    });

    setReceiverMessage(`Updated parser rule: ${editingParserRuleId}`, "ok");

    await loadAll();

    const receiver = selectedReceiver();
    if (receiver) {
      openReceiverSettings(receiver.id);
      activateReceiverSettingsTab("parsers");
    }

    selectedParserRuleId = editingParserRuleId;
    renderReceiverParserRules();
    clearParserRuleFormForReceiver();

  } catch (err) {
    setReceiverMessage(`Update parser rule failed: ${err.message}`, "error");
    updateParserRulePreviewForReceiver();
  }
}

async function copyParserRuleJsonForReceiver() {
  try {
    const rule = parserRuleFromFormForReceiver();
    const text = prettyJson(rule);

    if (navigator.clipboard?.writeText) {
      await navigator.clipboard.writeText(text);
    } else {
      const tmp = document.createElement("textarea");
      tmp.value = text;
      document.body.appendChild(tmp);
      tmp.select();
      document.execCommand("copy");
      tmp.remove();
    }

    setReceiverMessage("Parser JSON copied.", "ok");
  } catch (err) {
    setReceiverMessage(`Copy failed: ${err.message}`, "error");
  }
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

  renderRankList(
    topIpsWidget,
    topEntries(allEvents, (item) => eventField(item, "src_ip"), 8),
    "No source IPs found"
  );

  renderRankList(
    topUsersWidget,
    topEntries(allEvents, (item) => eventField(item, "user"), 8),
    "No users found"
  );

  renderAlertsTrend(alertsTrendWidget, allAlerts);

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

  const authEvents = allEvents.filter(isAuthEvent);
  const authAlerts = allAlerts.filter(isAuthAlert);

  const authFailed = authEvents.filter((item) => {
    const type = eventType(item);
    return type === "auth_failed" || type === "failed_login";
  });

  const authSuccess = authEvents.filter((item) => {
    const type = eventType(item);
    return type === "auth_success" || type === "accepted_login";
  });

  const authInvalid = authEvents.filter((item) => {
    const type = eventType(item);
    return type === "auth_invalid_user" || type === "invalid_user";
  });

  setTextSafe(authTotalEvents, formatNumber(authEvents.length));
  setTextSafe(authFailedEvents, formatNumber(authFailed.length));
  setTextSafe(authSuccessEvents, formatNumber(authSuccess.length));
  setTextSafe(authInvalidEvents, formatNumber(authInvalid.length));

  renderRankList(
    authTopIpsWidget,
    topEntries(authEvents, (item) => eventField(item, "src_ip"), 10),
    "No auth IPs found"
  );

  renderRankList(
    authTopUsersWidget,
    topEntries(authEvents, (item) => eventField(item, "user"), 10),
    "No auth users found"
  );

  renderBars(
    authTypesWidget,
    countBy(authEvents, (item) => eventField(item, "event_type")),
    false
  );

  renderRankList(
    authAlertsWidget,
    topEntries(authAlerts, (item) => alertField(item, "rule"), 8),
    "No auth alerts found"
  );

  const recentAuthEvents = authEvents.slice(0, 6);
  eventPools["auth-events"] = recentAuthEvents;

  authRecentEvents.innerHTML = recentAuthEvents
    .map((item, index) => renderEventCard(item, index, "auth-events"))
    .join("") || `<div class="empty">No auth events loaded</div>`;

  const fileEvents = allEvents.filter(isFileEvent);
  const fileAlerts = allAlerts.filter(isFileAlert);

  const fileCreated = fileEvents.filter((item) => eventType(item) === "file_created");
  const fileModified = fileEvents.filter((item) => eventType(item) === "file_modified");
  const fileDeleted = fileEvents.filter((item) => eventType(item) === "file_deleted");

  setTextSafe(fileTotalEvents, formatNumber(fileEvents.length));
  setTextSafe(fileCreatedEvents, formatNumber(fileCreated.length));
  setTextSafe(fileModifiedEvents, formatNumber(fileModified.length));
  setTextSafe(fileDeletedEvents, formatNumber(fileDeleted.length));

  renderRankList(
    fileTopPathsWidget,
    topEntries(fileEvents, (item) => eventField(item, "path"), 10),
    "No file paths found"
  );

  renderBars(
    fileActionsWidget,
    countBy(fileEvents, (item) => eventField(item, "event_type")),
    false
  );

  renderBars(
    fileSeverityWidget,
    countBy(fileEvents, (item) => eventField(item, "severity")),
    true
  );

  renderRankList(
    fileAlertsWidget,
    topEntries(fileAlerts, (item) => alertField(item, "rule"), 8),
    "No file alerts found"
  );

  const recentFileEvents = fileEvents.slice(0, 6);
  eventPools["file-events"] = recentFileEvents;

  fileRecentEvents.innerHTML = recentFileEvents
    .map((item, index) => renderEventCard(item, index, "file-events"))
    .join("") || `<div class="empty">No file events loaded</div>`;

  activateDashboardMode(currentDashboardMode);
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
    updateRulePreview();
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

  updateRulePreview();
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

function canAdmin() {
  return currentUser && currentUser.role === "admin";
}

function canEditRules() {
  return currentUser && currentUser.role === "admin";
}

function canViewRules() {
  return currentUser && (currentUser.role === "admin" || currentUser.role === "analyst");
}

function setUserMessage(message, type = "info") {
  if (!userMessage) return;
  userMessage.textContent = message || "";
  userMessage.className = `rule-message ${type}`;
}

function updateAuthUi() {
  const username = currentUser?.username || "—";
  const role = currentUser?.role || "—";

  document.body.dataset.role = role;

  document.body.classList.toggle(
    "password-required",
    Boolean(currentUser?.password_change_required)
  );

  if (currentUserText) currentUserText.textContent = username;
  if (currentRoleText) currentRoleText.textContent = `role: ${role}`;

  document.querySelectorAll(".admin-only").forEach((el) => {
    el.classList.toggle("hidden", !canAdmin());
  });

  const rulesBtn = document.querySelector('.nav-btn[data-tab="rules"]');
  if (rulesBtn) {
    rulesBtn.classList.toggle("hidden", !canViewRules());
  }

  if (!canViewRules() && document.querySelector("#tabRules.active")) {
    activateTab("dashboard");
  }

  if (!canAdmin() && document.querySelector("#tabReceivers.active")) {
    activateTab("dashboard");
  }

  if (!canAdmin() && document.querySelector("#tabUsers.active")) {
    activateTab("dashboard");
  }
}

function showLogin() {
  currentUser = null;

  if (loginScreen) loginScreen.classList.remove("hidden");
  if (appShell) appShell.classList.add("app-hidden");

  if (autoRefreshTimer) {
    clearInterval(autoRefreshTimer);
    autoRefreshTimer = null;
  }

  if (sideBrandMark) {
    sideBrandMark.textContent = "MS";
  }

  updateAuthUi();
}

function showApp() {
  if (loginScreen) loginScreen.classList.add("hidden");
  if (appShell) appShell.classList.remove("app-hidden");

  updateAuthUi();
  updateSideBrandMark();
  applySidebarState();

  if (currentUser?.password_change_required) {
    openPasswordModal(true);
  }
}

function renderUsers() {
  if (!usersList) return;

  if (usersCount) {
    usersCount.textContent = allUsers.length;
  }

  if (!allUsers.length) {
    usersList.innerHTML = `<div class="empty">No users found.</div>`;
    return;
  }

  usersList.innerHTML = allUsers.map((user) => {
    const enabled = user.enabled !== false;
    const role = escapeHtml(user.role || "viewer");
    const username = escapeHtml(user.username || "");
    const disabledClass = enabled ? "" : "disabled";

    return `
      <article class="user-card ${disabledClass}">
        <div class="user-main-row">
          <div>
            <div class="user-name">${username}</div>
            <div class="user-meta">
              role=${role}<br />
		enabled=${enabled ? "true" : "false"}<br />
		temporary_password=${user.password_change_required ? "true" : "false"}<br />
		created_at=${escapeHtml(user.created_at || "")}
            </div>
          </div>

          <span class="rule-state ${enabled ? "enabled" : "disabled"}">
            ${enabled ? "enabled" : "disabled"}
          </span>
        </div>

        <div class="user-actions">
          <button onclick="changeUserRole('${escapeAttr(user.username)}')">Role</button>
          <button onclick="toggleUserEnabled('${escapeAttr(user.username)}')">${enabled ? "Disable" : "Enable"}</button>
          <button onclick="resetUserPassword('${escapeAttr(user.username)}')">Password</button>
          <button class="delete" onclick="deleteUser('${escapeAttr(user.username)}')">Delete</button>
        </div>
      </article>
    `;
  }).join("");
}

function renderAll() {
  closeAllPanels();
  renderDashboard();
  renderEvents();
  renderAlerts();

  if (canAdmin()) {
    renderReceivers();
  }

  if (canViewRules()) {
    renderRules();
  }

  if (canAdmin()) {
    renderUsers();
  }

  if (currentCustomDashboard) {
    renderCustomDashboardBuilder();
  }

  updateRulePreview();
}

async function loadAll(options = {}) {
  const silent = options.silent === true;

  try {
    if (!silent) setStatus("Loading...");

    const limit = getLimit();

    const baseRequests = [
      fetchJson("/api/stats"),
      fetchJson(`/api/events?limit=${limit}`),
      fetchJson(`/api/alerts?limit=${limit}`)
    ];

    if (canViewRules()) {
      baseRequests.push(fetchJson("/api/rules"));
    } else {
      baseRequests.push(Promise.resolve({ rules: [] }));
    }

    baseRequests.push(fetchJson("/api/dashboards"));

    if (canAdmin()) {
      baseRequests.push(fetchJson("/api/users"));
      baseRequests.push(fetchJson("/api/receivers"));
      baseRequests.push(fetchJson("/api/policy-groups"));
      baseRequests.push(fetchJson("/api/parser-rules"));
      baseRequests.push(fetchJson("/api/field-catalog"));
    } else {
      baseRequests.push(Promise.resolve({ users: [] }));
      baseRequests.push(Promise.resolve({ receivers: [] }));
      baseRequests.push(Promise.resolve({ policy_groups: [] }));
      baseRequests.push(Promise.resolve({ parser_rules: [] }));
      baseRequests.push(Promise.resolve({ catalog: { fields: [] } }));
    }

    const [
      statsData,
      eventsData,
      alertsData,
      rulesData,
      dashboardsData,
      usersData,
      receiversData,
      policyGroupsData,
      parserRulesData,
      fieldCatalogData
    ] = await Promise.all(baseRequests);

    stats = statsData || {};
    allEvents = Array.isArray(eventsData.events) ? eventsData.events : [];
    allAlerts = Array.isArray(alertsData.alerts) ? alertsData.alerts : [];
    allRules = Array.isArray(rulesData.rules) ? rulesData.rules : [];
    allUsers = Array.isArray(usersData.users) ? usersData.users : [];

    allReceivers = Array.isArray(receiversData.receivers) ? receiversData.receivers : [];
    allPolicyGroups = Array.isArray(policyGroupsData.policy_groups) ? policyGroupsData.policy_groups : [];
    allParserRules = Array.isArray(parserRulesData.parser_rules) ? parserRulesData.parser_rules : [];
    fieldCatalog = Array.isArray(fieldCatalogData.catalog?.fields)
      ? fieldCatalogData.catalog.fields
      : [];

    customDashboards = Array.isArray(dashboardsData.dashboards)
      ? dashboardsData.dashboards.map(normalizeDashboard)
      : [];

    if (!currentCustomDashboard) {
      const preferredId = localStorage.getItem("mini_siem_custom_dashboard_id") || "";
      const preferred = customDashboards.find((dashboard) => dashboard.id === preferredId);
      const first = customDashboards[0];

      currentCustomDashboard = preferred || first || createBlankDashboard();
      selectedCustomDashboardId = currentCustomDashboard.id;
    } else {
      const refreshed = customDashboards.find((dashboard) => dashboard.id === currentCustomDashboard.id);
      if (refreshed) {
        currentCustomDashboard = refreshed;
        selectedCustomDashboardId = refreshed.id;
      }
    }

    renderAll();

    if (selectedReceiverId) {
      const stillExists = allReceivers.some((receiver) => receiver.id === selectedReceiverId);

      if (stillExists) {
        renderSelectedReceiverWorkspace();
      } else {
        closeReceiverSettings();
      }
    }

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

function setValidationMessage(message, type = "info") {
  if (!ruleValidationBox) return;

  ruleValidationBox.textContent = message || "";
  ruleValidationBox.className = `validation-box ${type}`;
}

function buildRuleFromForm(options = {}) {
  const allowEmptyId = options.allowEmptyId === true;
  const id = ruleIdInput.value.trim();

  if (!id && !allowEmptyId) {
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
    title: ruleTitleInput.value.trim() || id || "Untitled detection rule",
    description: ruleDescriptionInput.value.trim(),
    conditions: cloneJson(currentConditions)
  };
}

function validateRuleObject(rule, options = {}) {
  const skipDuplicateCheck = options.skipDuplicateCheck === true;
  const errors = [];
  const warnings = [];

  if (!rule.id || !String(rule.id).trim()) {
    errors.push("Rule ID is required.");
  }

  if (rule.id && !/^[A-Z0-9_:-]+$/i.test(rule.id)) {
    warnings.push("Rule ID contains unusual characters. Prefer letters, numbers and underscores.");
  }

  if (!rule.type) {
    errors.push("Rule type is required.");
  }

  if (rule.type !== "threshold" && rule.type !== "match") {
    errors.push("Rule type must be threshold or match.");
  }

  if (!rule.severity) {
    errors.push("Severity is required.");
  }

  if (!["info", "low", "medium", "high", "critical"].includes(rule.severity)) {
    errors.push("Severity must be info, low, medium, high or critical.");
  }

  if (!rule.title || !String(rule.title).trim()) {
    warnings.push("Title is empty. The UI will use Rule ID as title.");
  }

  if (!Array.isArray(rule.event_types) || !rule.event_types.length) {
    warnings.push("No event types selected. Rule will match any event_type.");
  }

  if (!Array.isArray(rule.source_types) || !rule.source_types.length) {
    warnings.push("No source types selected. Rule will match any source_type.");
  }

  if (rule.type === "threshold") {
    if (!rule.group_by) {
      errors.push("Threshold rules require group_by.");
    }

    if (!Number.isInteger(rule.threshold) || rule.threshold < 1) {
      errors.push("Threshold must be an integer >= 1.");
    }

    if (!Number.isInteger(rule.window_sec) || rule.window_sec < 1) {
      errors.push("Window seconds must be an integer >= 1.");
    }
  }

  if (rule.type === "match") {
    if (!rule.group_by) {
      warnings.push("Match rule has no group_by. Event ID will be used for suppression grouping.");
    }
  }

  if (!Number.isInteger(rule.suppress_sec) || rule.suppress_sec < 0) {
    errors.push("Suppress seconds must be an integer >= 0.");
  }

  if (Array.isArray(rule.conditions)) {
    rule.conditions.forEach((condition, index) => {
      if (!condition.field) {
        errors.push(`Condition #${index + 1} has empty field.`);
      }

      if (!condition.op) {
        errors.push(`Condition #${index + 1} has empty operator.`);
      }

      if (condition.op === "in" || condition.op === "contains_any") {
        if (!Array.isArray(condition.values) || !condition.values.length) {
          errors.push(`Condition #${index + 1} requires values.`);
        }
      } else if (condition.op !== "exists") {
        if (!condition.value) {
          warnings.push(`Condition #${index + 1} has empty value.`);
        }
      }
    });
  }

  if (!skipDuplicateCheck && !editingRuleId && rule.id) {
    const exists = allRules.some((item) => item.id === rule.id);
    if (exists) {
      errors.push(`Rule with ID ${rule.id} already exists.`);
    }
  }

  return {
    ok: errors.length === 0,
    errors,
    warnings
  };
}

function updateRulePreview() {
  if (!ruleJsonPreview) return;

  let rule;
  try {
    rule = buildRuleFromForm({ allowEmptyId: true });
  } catch (err) {
    ruleJsonPreview.textContent = "{}";
    setValidationMessage(err.message, "error");
    return;
  }

  ruleJsonPreview.textContent = prettyJson(rule);

  const validation = validateRuleObject(rule, {
    skipDuplicateCheck: true
  });

  if (!rule.id) {
    setValidationMessage("Fill Rule ID to make the rule valid.", "info");
    return;
  }

  if (!validation.ok) {
    setValidationMessage(validation.errors[0], "error");
    return;
  }

  if (validation.warnings.length) {
    setValidationMessage(validation.warnings[0], "warn");
    return;
  }

  setValidationMessage("Rule preview is valid.", "ok");
}

function ruleFromForm() {
  const rule = buildRuleFromForm();
  const validation = validateRuleObject(rule);

  if (!validation.ok) {
    throw new Error(validation.errors.join(" "));
  }

  return rule;
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
    ? cloneJson(rule.conditions)
    : [];

  renderConditions();

  createRuleBtn.disabled = true;
  updateRuleBtn.disabled = false;

  if (editorModePill) {
    editorModePill.textContent = "edit mode";
  }

  setRuleMessage(`Editing ${editingRuleId}`, "info");
  updateRulePreview();
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

  if (editorModePill) {
    editorModePill.textContent = "create mode";
  }

  setRuleMessage("", "info");
  updateRulePreview();
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

function loadRuleTemplate(templateName) {
  const template = ruleTemplates[templateName];

  if (!template) {
    setRuleMessage(`Unknown template: ${templateName}`, "error");
    return;
  }

  const rule = cloneJson(template);

  if (allRules.some((item) => item.id === rule.id)) {
    const suffix = String(Date.now()).slice(-5);
    rule.id = `${rule.id}_${suffix}`;
  }

  editingRuleId = "";
  ruleFormTitle.textContent = "Create rule from template";
  ruleIdInput.disabled = false;
  ruleIdInput.value = rule.id;

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
    ? cloneJson(rule.conditions)
    : [];

  createRuleBtn.disabled = false;
  updateRuleBtn.disabled = true;

  if (editorModePill) {
    editorModePill.textContent = "template mode";
  }

  renderConditions();
  updateRulePreview();
  setRuleMessage(`Template loaded: ${templateName}`, "ok");
}

function validateCurrentRule() {
  try {
    const rule = buildRuleFromForm();
    const validation = validateRuleObject(rule);

    updateRulePreview();

    if (!validation.ok) {
      setRuleMessage(`Validation failed: ${validation.errors.join(" ")}`, "error");
      setValidationMessage(validation.errors.join(" "), "error");
      return false;
    }

    if (validation.warnings.length) {
      setRuleMessage(`Rule valid with warning: ${validation.warnings[0]}`, "info");
      setValidationMessage(validation.warnings[0], "warn");
      return true;
    }

    setRuleMessage("Rule is valid", "ok");
    setValidationMessage("Rule is valid and ready to save.", "ok");
    return true;
  } catch (err) {
    setRuleMessage(`Validation failed: ${err.message}`, "error");
    setValidationMessage(err.message, "error");
    return false;
  }
}

async function copyRuleJson() {
  try {
    const rule = buildRuleFromForm({ allowEmptyId: true });
    const text = prettyJson(rule);

    if (navigator.clipboard?.writeText) {
      await navigator.clipboard.writeText(text);
    } else {
      const tmp = document.createElement("textarea");
      tmp.value = text;
      document.body.appendChild(tmp);
      tmp.select();
      document.execCommand("copy");
      tmp.remove();
    }

    setRuleMessage("Rule JSON copied", "ok");
    setValidationMessage("Copied JSON to clipboard.", "ok");
  } catch (err) {
    setRuleMessage(`Copy failed: ${err.message}`, "error");
  }
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
  if (!canEditRules()) {
    setRuleMessage("Only admin can change rules.", "error");
    return;
  }

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
    updateRulePreview();
  }
}

async function updateRule() {
  if (!canEditRules()) {
    setRuleMessage("Only admin can change rules.", "error");
    return;
  }

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
    updateRulePreview();
  }
}

async function toggleRule(id) {
  if (!canEditRules()) {
    setRuleMessage("Only admin can change rules.", "error");
    return;
  }

  try {
    const rule = allRules.find((item) => item.id === id);

    if (!rule) {
      throw new Error(`Rule not found: ${id}`);
    }

    const updated = cloneJson(rule);
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
  if (!canEditRules()) {
    setRuleMessage("Only admin can change rules.", "error");
    return;
  }

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

  const icon = currentTheme === "cute" ? "☀" : "🌙";
  const label = currentTheme === "cute"
    ? "Current theme: cute pink"
    : "Current theme: horror red";

  if (themeToggleBtn) {
    themeToggleBtn.textContent = icon;
    themeToggleBtn.title = label;
    themeToggleBtn.setAttribute("aria-label", label);
  }

  if (loginThemeToggleBtn) {
    loginThemeToggleBtn.textContent = icon;
    loginThemeToggleBtn.title = label;
    loginThemeToggleBtn.setAttribute("aria-label", label);
  }

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

function bindRulePreviewEvents() {
  const previewInputs = [
    ruleIdInput,
    ruleEnabledInput,
    ruleTypeInput,
    ruleSeverityInput,
    ruleEventTypesInput,
    ruleSourceTypesInput,
    ruleGroupByInput,
    ruleThresholdInput,
    ruleWindowInput,
    ruleSuppressInput,
    ruleTitleInput,
    ruleDescriptionInput
  ];

  previewInputs.forEach((input) => {
    if (!input) return;
    input.addEventListener("input", updateRulePreview);
    input.addEventListener("change", updateRulePreview);
  });

  document.querySelectorAll("[data-rule-template]").forEach((btn) => {
    btn.addEventListener("click", () => {
      loadRuleTemplate(btn.dataset.ruleTemplate);
    });
  });

  if (validateRuleBtn) {
    validateRuleBtn.addEventListener("click", validateCurrentRule);
  }

  if (copyRuleJsonBtn) {
    copyRuleJsonBtn.addEventListener("click", copyRuleJson);
  }
}

function bindEvents() {
  document.querySelectorAll(".nav-btn").forEach((btn) => {
    btn.addEventListener("click", () => {
      activateTab(btn.dataset.tab);
    });
  });

  document.querySelectorAll(".dashboard-mode-btn").forEach((btn) => {
    btn.addEventListener("click", () => {
      activateDashboardMode(btn.dataset.dashboardMode);
    });
  });

  refreshBtn.addEventListener("click", () => loadAll());
  autoRefreshBtn.addEventListener("click", toggleAutoRefresh);

  if (themeToggleBtn) {
    themeToggleBtn.addEventListener("click", toggleTheme);
  }

  if (loginThemeToggleBtn) {
    loginThemeToggleBtn.addEventListener("click", toggleTheme);
  }

  if (sidebarToggleBtn) {
    sidebarToggleBtn.addEventListener("click", toggleSidebar);
  }

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

  if (customDashboardSelect) {
    customDashboardSelect.addEventListener("change", () => {
      selectCustomDashboard(customDashboardSelect.value);
    });
  }

  if (newDashboardBtn) {
    newDashboardBtn.addEventListener("click", newCustomDashboard);
  }

  if (duplicateDashboardBtn) {
    duplicateDashboardBtn.addEventListener("click", duplicateCustomDashboard);
  }

  if (saveDashboardBtn) {
    saveDashboardBtn.addEventListener("click", saveCustomDashboard);
  }

  if (deleteDashboardBtn) {
    deleteDashboardBtn.addEventListener("click", deleteCustomDashboard);
  }

  if (addWidgetBtn) {
    addWidgetBtn.addEventListener("click", addCustomWidget);
  }

  if (updateWidgetBtn) {
    updateWidgetBtn.addEventListener("click", updateCustomWidget);
  }

  if (clearWidgetFormBtn) {
    clearWidgetFormBtn.addEventListener("click", clearWidgetForm);
  }

  if (loginForm) {
    loginForm.addEventListener("submit", loginUser);
  }

  if (logoutBtn) {
    logoutBtn.addEventListener("click", logoutUser);
  }

  if (changePasswordOpenBtn) {
    changePasswordOpenBtn.addEventListener("click", () => openPasswordModal(false));
  }

  if (changePasswordBtn) {
    changePasswordBtn.addEventListener("click", changeOwnPassword);
  }

  if (cancelPasswordBtn) {
    cancelPasswordBtn.addEventListener("click", closePasswordModal);
  }

  if (createUserBtn) {
    createUserBtn.addEventListener("click", createUser);
  }

  if (clearUserFormBtn) {
    clearUserFormBtn.addEventListener("click", clearUserForm);
  }

  [
    customDashboardTitleInput,
    customDashboardDescriptionInput,
    customDashboardColumnsInput
  ].forEach((input) => {
    if (!input) return;

    input.addEventListener("input", () => {
      if (!currentCustomDashboard) return;
      currentCustomDashboard = currentDashboardFromForm();
      renderCustomDashboardBuilder();
    });

    input.addEventListener("change", () => {
      if (!currentCustomDashboard) return;
      currentCustomDashboard = currentDashboardFromForm();
      renderCustomDashboardBuilder();
    });
  });

  if (widgetTypeInput) {
    widgetTypeInput.addEventListener("change", () => {
      if (!widgetTitleInput.value.trim()) {
        widgetTitleInput.value = defaultWidgetTitles[widgetTypeInput.value] || "";
      }
    });
  }

  if (newReceiverBtn) {
    newReceiverBtn.addEventListener("click", newReceiver);
  }

  if (deleteSelectedReceiverBtn) {
    deleteSelectedReceiverBtn.addEventListener("click", () => {
      deleteReceiver(selectedReceiverId);
    });
  }

  if (openSelectedReceiverSettingsBtn) {
    openSelectedReceiverSettingsBtn.addEventListener("click", () => {
      openReceiverSettings(selectedReceiverId);
    });
  }

  if (openReceiverSettingsInlineBtn) {
    openReceiverSettingsInlineBtn.addEventListener("click", () => {
      openReceiverSettings(selectedReceiverId);
    });
  }

  if (closeReceiverSettingsBtn) {
    closeReceiverSettingsBtn.addEventListener("click", closeReceiverSettings);
  }

  if (saveReceiverBtn) {
    saveReceiverBtn.addEventListener("click", saveReceiver);
  }

  if (deleteReceiverBtn) {
    deleteReceiverBtn.addEventListener("click", () => {
      deleteReceiver(editingReceiverId || selectedReceiverId);
    });
  }

  if (saveReceiverDashboardsBtn) {
    saveReceiverDashboardsBtn.addEventListener("click", saveReceiverDashboards);
  }

  if (receiverTreeSearchInput) {
    receiverTreeSearchInput.addEventListener("input", () => {
      receiverTreeSearch = receiverTreeSearchInput.value || "";
      renderReceivers();
    });
  }

if (receiverParserStatusFilterInput) {
  receiverParserStatusFilterInput.addEventListener("change", () => {
    updateReceiverFilterStateFromInputs();
    renderSelectedReceiverWorkspace();
  });
}

if (receiverKqlFilterInput) {
  receiverKqlFilterInput.addEventListener("input", () => {
    updateReceiverFilterStateFromInputs();
    renderSelectedReceiverWorkspace();
  });
}

if (clearReceiverFiltersBtn) {
  clearReceiverFiltersBtn.addEventListener("click", clearReceiverFilters);
}

  document.querySelectorAll(".receiver-settings-tab").forEach((btn) => {
    btn.addEventListener("click", () => {
      activateReceiverSettingsTab(btn.dataset.receiverSettingsTab);
    });
  });

  document.querySelectorAll(".receiver-dashboard-tab").forEach((btn) => {
    btn.addEventListener("click", () => {
      activateReceiverDashboardTab(btn.dataset.receiverDashboardTab);
    });
  });

[
  receiverIdInput,
  receiverEnabledInput,
  receiverNameInput,
  receiverRoleInput,
  receiverParentInput,
  receiverTypeInput,
  receiverPathInput,
  receiverFilePatternInput,
  receiverRecursiveInput,
  receiverSourceTypeInput,
  receiverInitialPositionInput
].forEach((input) => {
    if (!input) return;
    input.addEventListener("input", () => setReceiverMessage(""));
    input.addEventListener("change", () => setReceiverMessage(""));
  });

  if (receiverPolicyGroupsList) {
    receiverPolicyGroupsList.addEventListener("change", () => setReceiverMessage(""));
  }

  [
    parserRuleIdInput,
    parserRuleEnabledInput,
    parserRuleNameInput,
    parserPolicyGroupInput,
    parserMatchTypeInput,
    parserRawSampleInput,
    parserPatternInput,
    parserFieldOrderInput,
    parserConstantEventCodeInput,
    parserConstantEventNameInput,
    parserConstantEventTypeInput,
    parserConstantEventCategoryInput,
    parserConstantEventActionInput,
    parserConstantEventOutcomeInput,
    parserConstantSeverityInput,
    parserConstantVendorInput,
    parserConstantProductInput
  ].forEach((input) => {
    if (!input) return;
    input.addEventListener("input", updateParserRulePreviewForReceiver);
    input.addEventListener("change", updateParserRulePreviewForReceiver);
  });

  if (parserReceiversList) {
    parserReceiversList.addEventListener("change", updateParserRulePreviewForReceiver);
  }

  if (parserMatchTypeInput) {
    parserMatchTypeInput.addEventListener("change", () => {
      updateParserModeHintForReceiver();
      currentParserExtracted = {};
      currentParserMapping = {};

      if (parserExtractedPreview) {
        parserExtractedPreview.textContent = "No extraction test yet.";
      }

      renderParserMappingForReceiver();
      updateParserRulePreviewForReceiver();
    });
  }

  if (testParserRuleBtn) {
    testParserRuleBtn.addEventListener("click", testParserRuleForReceiver);
  }

  if (copyParserRuleJsonBtn) {
    copyParserRuleJsonBtn.addEventListener("click", copyParserRuleJsonForReceiver);
  }

  if (clearParserRuleFormBtn) {
    clearParserRuleFormBtn.addEventListener("click", clearParserRuleFormForReceiver);
  }

  if (newParserRuleFromSettingsBtn) {
    newParserRuleFromSettingsBtn.addEventListener("click", newParserRuleFromSettings);
  }

  if (deleteSelectedParserRuleBtn) {
    deleteSelectedParserRuleBtn.addEventListener("click", deleteSelectedParserRuleFromSettings);
  }

  if (editSelectedParserRuleBtn) {
    editSelectedParserRuleBtn.addEventListener("click", editSelectedParserRuleFromSettings);
  }

if (toggleSelectedParserRuleBtn) {
  toggleSelectedParserRuleBtn.addEventListener("click", toggleSelectedParserRuleFromSettings);
}

  if (createParserRuleBtn) {
    createParserRuleBtn.addEventListener("click", createParserRuleForReceiver);
  }

  if (updateParserRuleBtn) {
    updateParserRuleBtn.addEventListener("click", updateParserRuleForReceiver);
  }

  bindRulePreviewEvents();
}

function clearPasswordForm() {
  if (oldPasswordInput) oldPasswordInput.value = "";
  if (newOwnPasswordInput) newOwnPasswordInput.value = "";
  if (repeatOwnPasswordInput) repeatOwnPasswordInput.value = "";

  if (passwordMessage) {
    passwordMessage.textContent = "";
    passwordMessage.className = "login-message";
  }
}

function openPasswordModal(required = false) {
  clearPasswordForm();

  if (passwordModalText) {
    passwordModalText.textContent = required
      ? "This password is temporary. Please change it before using Mini SIEM."
      : "Update your account password.";
  }

  if (passwordModal) {
    passwordModal.classList.remove("hidden");
  }

  document.body.classList.toggle("password-required", required);
}

function closePasswordModal() {
  if (currentUser?.password_change_required) {
    return;
  }

  if (passwordModal) {
    passwordModal.classList.add("hidden");
  }

  clearPasswordForm();
}

async function changeOwnPassword() {
  try {
    const oldPassword = oldPasswordInput.value;
    const newPassword = newOwnPasswordInput.value;
    const repeatPassword = repeatOwnPasswordInput.value;

    if (!oldPassword || !newPassword || !repeatPassword) {
      throw new Error("All password fields are required.");
    }

    if (newPassword !== repeatPassword) {
      throw new Error("New passwords do not match.");
    }

    if (newPassword.length < 6) {
      throw new Error("Password must contain at least 6 characters.");
    }

    await fetchJson("/api/auth/change-password", {
      method: "POST",
      headers: {
        "Content-Type": "application/json"
      },
      body: JSON.stringify({
        old_password: oldPassword,
        new_password: newPassword
      })
    });

    if (currentUser) {
      currentUser.password_change_required = false;
    }

    document.body.classList.remove("password-required");

    if (passwordModal) {
      passwordModal.classList.add("hidden");
    }

    clearPasswordForm();
    updateAuthUi();
    await loadAll();

  } catch (err) {
    if (passwordMessage) {
      passwordMessage.textContent = err.message;
      passwordMessage.className = "login-message error";
    }
  }
}

async function checkAuth() {
  try {
    const data = await fetchJson("/api/auth/me");
    currentUser = data.user || null;

    showApp();
    await loadAll();
  } catch (_) {
    showLogin();
  }
}

async function loginUser(event) {
  event.preventDefault();

  if (loginMessage) {
    loginMessage.textContent = "Signing in...";
    loginMessage.className = "login-message";
  }

  try {
    const body = {
      username: loginUsernameInput.value.trim(),
      password: loginPasswordInput.value
    };

    const data = await fetchJson("/api/auth/login", {
      method: "POST",
      headers: {
        "Content-Type": "application/json"
      },
      body: JSON.stringify(body)
    });

    currentUser = data.user || null;

    if (loginPasswordInput) {
      loginPasswordInput.value = "";
    }

    if (loginMessage) {
      loginMessage.textContent = "";
      loginMessage.className = "login-message";
    }

    showApp();
    await loadAll();
  } catch (err) {
    if (loginMessage) {
      loginMessage.textContent = err.message;
      loginMessage.className = "login-message error";
    }
  }
}

async function logoutUser() {
  try {
    await fetchJson("/api/auth/logout", {
      method: "POST"
    });
  } catch (_) {
    // logout should still clear local UI
  }

  showLogin();
}

function clearUserForm() {
  if (newUsernameInput) newUsernameInput.value = "";
  if (newPasswordInput) newPasswordInput.value = "";
  if (newRoleInput) newRoleInput.value = "viewer";
  if (newEnabledInput) newEnabledInput.checked = true;
  setUserMessage("");
}

async function loadUsersOnly() {
  if (!canAdmin()) return;

  const data = await fetchJson("/api/users");
  allUsers = Array.isArray(data.users) ? data.users : [];
  renderUsers();
}

async function createUser() {
  if (!canAdmin()) return;

  try {
    const body = {
      username: newUsernameInput.value.trim(),
      password: newPasswordInput.value,
      role: newRoleInput.value || "viewer",
      enabled: newEnabledInput.checked
    };

    await fetchJson("/api/users", {
      method: "POST",
      headers: {
        "Content-Type": "application/json"
      },
      body: JSON.stringify(body)
    });

    clearUserForm();
    await loadUsersOnly();
    setUserMessage("User created.", "ok");
  } catch (err) {
    setUserMessage(err.message, "error");
  }
}

async function changeUserRole(username) {
  if (!canAdmin()) return;

  const user = allUsers.find((item) => item.username === username);
  if (!user) return;

  const nextRole = prompt("New role: admin / analyst / viewer", user.role || "viewer");
  if (!nextRole) return;

  const normalizedRole = nextRole.trim();

  if (!["admin", "analyst", "viewer"].includes(normalizedRole)) {
    alert("Invalid role");
    return;
  }

  try {
    await fetchJson(`/api/users/${encodeURIComponent(username)}`, {
      method: "PUT",
      headers: {
        "Content-Type": "application/json"
      },
      body: JSON.stringify({
        role: normalizedRole,
        enabled: user.enabled !== false
      })
    });

    await loadUsersOnly();
    setUserMessage("User role updated.", "ok");
  } catch (err) {
    setUserMessage(err.message, "error");
  }
}

async function toggleUserEnabled(username) {
  if (!canAdmin()) return;

  const user = allUsers.find((item) => item.username === username);
  if (!user) return;

  if (currentUser && currentUser.username === username && user.enabled !== false) {
    alert("You cannot disable your current user.");
    return;
  }

  try {
    await fetchJson(`/api/users/${encodeURIComponent(username)}`, {
      method: "PUT",
      headers: {
        "Content-Type": "application/json"
      },
      body: JSON.stringify({
        role: user.role || "viewer",
        enabled: !(user.enabled !== false)
      })
    });

    await loadUsersOnly();
    setUserMessage("User updated.", "ok");
  } catch (err) {
    setUserMessage(err.message, "error");
  }
}

async function resetUserPassword(username) {
  if (!canAdmin()) return;

  const password = prompt(`New password for ${username}:`);
  if (!password) return;

  try {
    await fetchJson(`/api/users/${encodeURIComponent(username)}/password`, {
      method: "POST",
      headers: {
        "Content-Type": "application/json"
      },
      body: JSON.stringify({
        password
      })
    });

    setUserMessage("Password updated.", "ok");
  } catch (err) {
    setUserMessage(err.message, "error");
  }
}

async function deleteUser(username) {
  if (!canAdmin()) return;

  if (currentUser && currentUser.username === username) {
    alert("You cannot delete your current user.");
    return;
  }

  if (!confirm(`Delete user ${username}?`)) return;

  try {
    await fetchJson(`/api/users/${encodeURIComponent(username)}`, {
      method: "DELETE"
    });

    await loadUsersOnly();
    setUserMessage("User deleted.", "ok");
  } catch (err) {
    setUserMessage(err.message, "error");
  }
}

window.toggleEventPanel = toggleEventPanel;
window.toggleAlertPanel = toggleAlertPanel;
window.toggleRuleDetails = toggleRuleDetails;
window.editRule = editRule;
window.toggleRule = toggleRule;
window.deleteRule = deleteRule;
window.removeCondition = removeCondition;
window.copyPreBlock = copyPreBlock;

window.openReceiverSettings = openReceiverSettings;
window.toggleParserRuleForSelectedReceiver = toggleParserRuleForSelectedReceiver;
window.editParserRuleForReceiver = editParserRuleForReceiver;

window.deleteParserRuleForReceiver = deleteParserRuleForReceiver;

window.newParserRuleFromSettings = newParserRuleFromSettings;
window.editSelectedParserRuleFromSettings = editSelectedParserRuleFromSettings;
window.deleteSelectedParserRuleFromSettings = deleteSelectedParserRuleFromSettings;

window.toggleSelectedParserRuleFromSettings = toggleSelectedParserRuleFromSettings;

window.editCustomWidget = editCustomWidget;
window.removeCustomWidget = removeCustomWidget;
window.moveCustomWidget = moveCustomWidget;

window.selectCustomDashboard = selectCustomDashboard;
window.duplicateDashboardById = duplicateDashboardById;

applyTheme();
applySidebarState();
updateSideBrandMark();
updateAutoRefreshUi();
bindEvents();
renderConditions();
clearParserRuleFormForReceiver();
activateTab("dashboard");
activateDashboardMode(currentDashboardMode);
updateRulePreview();
updateParserRulePreviewForReceiver();
checkAuth();
