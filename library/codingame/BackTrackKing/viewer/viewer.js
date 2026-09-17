"use strict";

const TERRAIN = ["#b9d58c", "#5aa7d8", "#9a8470"];
const TERRAIN_NAME = ["plains", "river", "mountain"];
const OWNER = ["#e5484d", "#3e8ef7", "#b5b5b5"];
const BUDGET_FIRST = 1000;
const BUDGET = 50;
const DIRS = [[0, -1], [1, 0], [0, 1], [-1, 0]];

const $ = (id) => document.getElementById(id);
const canvas = $("map");
const ctx = canvas.getContext("2d");

let replay = null;
let townAt = new Map();
let turn = 0;
let timer = null;
let selected = null;
let cell = 32;

function key(x, y) { return y * 1000 + x; }

async function load() {
  stop();
  const url = $("replay").value;
  try {
    const res = await fetch(url + "?t=" + Date.now());
    if (!res.ok) throw new Error(res.status + " " + res.statusText);
    replay = await res.json();
  } catch (e) {
    replay = null;
    $("result").className = "result loss";
    $("result").textContent = "cannot load " + url + " (" + e.message + ") — run make test first";
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    return;
  }
  townAt = new Map(replay.towns.map((t) => [key(t.x, t.y), t]));
  selected = null;
  $("scrub").max = replay.turns.length - 1;
  resize();
  const s = replay.summary;
  const r = $("result");
  r.className = "result " + (s.draw ? "" : s.you_won ? "win" : "loss");
  r.textContent = `League ${s.league} · seed ${s.seed} · ` +
    (s.draw ? "DRAW" : s.you_won ? "WIN" : "LOSS") + ` · ${s.end_reason}`;
  $("name0").textContent = label(0);
  $("name1").textContent = label(1);
  show(Number(new URLSearchParams(location.search).get("turn")) || 0);
}

function label(p) {
  const name = replay.summary.players[p] || "";
  const base = name.split("/").pop();
  return (p === replay.summary.you ? "you " : "") + base;
}

function resize() {
  if (!replay) return;
  const width = canvas.parentElement.clientWidth;
  cell = Math.max(12, Math.floor(width / replay.width));
  const dpr = window.devicePixelRatio || 1;
  canvas.width = replay.width * cell * dpr;
  canvas.height = replay.height * cell * dpr;
  canvas.style.width = replay.width * cell + "px";
  canvas.style.height = replay.height * cell + "px";
  ctx.setTransform(dpr, 0, 0, dpr, 0, 0);
  draw();
}

function show(t) {
  if (!replay) return;
  turn = Math.max(0, Math.min(replay.turns.length - 1, t));
  $("scrub").value = turn;
  $("turn").textContent = `turn ${turn} / ${replay.turns.length - 1}`;
  const T = replay.turns[turn];
  $("score0").textContent = T.scores[0];
  $("score1").textContent = T.scores[1];
  for (const p of [0, 1]) {
    $("cmd" + p).textContent = T.commands ? (T.commands[p] ?? "") : "";
    const ms = turn > 0 && T.timing_ms ? T.timing_ms[p] : null;
    const em = $("ms" + p);
    const budget = turn <= 1 ? BUDGET_FIRST : BUDGET;
    em.textContent = ms == null ? "" : ms.toFixed(1) + " ms";
    em.className = ms == null ? "" : ms > budget ? "over" : ms > budget * 0.8 ? "slow" : "";
    $("err" + p).textContent = T.stderr ? T.stderr[p] || "" : "";
  }
  const msgs = (T.messages || []).map((m, p) => (m ? `P${p}: ${m}` : "")).filter(Boolean);
  $("msgs").textContent = msgs.join("  ·  ");
  $("log").textContent = (T.log || []).join("\n");
  renderConnections(T);
  draw();
}

function renderConnections(T) {
  const table = $("conns");
  table.innerHTML = "<tr><th>from</th><th>to</th><th>len</th><th>P0</th><th>P1</th></tr>";
  const active = new Map(T.connections.map((c) => [c.from + "-" + c.to, c]));
  for (const town of replay.towns) {
    for (const to of town.desired) {
      const id = town.id + "-" + to;
      const c = active.get(id);
      const tr = document.createElement("tr");
      tr.className = "row" + (c ? "" : " idle") + (selected === id ? " sel" : "");
      tr.innerHTML = c
        ? `<td>${town.id}</td><td>${to}</td><td>${c.path.length}</td><td class="pts0">${c.points[0]}</td><td class="pts1">${c.points[1]}</td>`
        : `<td>${town.id}</td><td>${to}</td><td colspan="3">not connected</td>`;
      tr.onclick = () => { selected = selected === id ? null : id; renderConnections(T); draw(); };
      table.appendChild(tr);
    }
  }
}

function trackAt(T, x, y) {
  const ch = T.tracks[y][x];
  return ch === "." ? -1 : Number(ch);
}

function draw() {
  if (!replay) return;
  const T = replay.turns[turn];
  const W = replay.width, H = replay.height, c = cell;
  const inked = new Set(T.inked);
  const regionOf = (x, y) => replay.regions[y][x];

  for (let y = 0; y < H; y++) {
    for (let x = 0; x < W; x++) {
      ctx.fillStyle = TERRAIN[Number(replay.types[y][x])];
      ctx.fillRect(x * c, y * c, c, c);
      if (inked.has(regionOf(x, y))) {
        ctx.fillStyle = "rgba(12, 12, 20, 0.82)";
        ctx.fillRect(x * c, y * c, c, c);
      }
    }
  }

  ctx.strokeStyle = "rgba(0, 0, 0, 0.12)";
  ctx.lineWidth = 1;
  ctx.beginPath();
  for (let x = 0; x <= W; x++) { ctx.moveTo(x * c + 0.5, 0); ctx.lineTo(x * c + 0.5, H * c); }
  for (let y = 0; y <= H; y++) { ctx.moveTo(0, y * c + 0.5); ctx.lineTo(W * c, y * c + 0.5); }
  ctx.stroke();

  ctx.strokeStyle = "rgba(20, 20, 30, 0.85)";
  ctx.lineWidth = 2;
  ctx.beginPath();
  for (let y = 0; y < H; y++) {
    for (let x = 0; x < W; x++) {
      if (x + 1 < W && regionOf(x, y) !== regionOf(x + 1, y)) {
        ctx.moveTo((x + 1) * c, y * c); ctx.lineTo((x + 1) * c, (y + 1) * c);
      }
      if (y + 1 < H && regionOf(x, y) !== regionOf(x, y + 1)) {
        ctx.moveTo(x * c, (y + 1) * c); ctx.lineTo((x + 1) * c, (y + 1) * c);
      }
    }
  }
  ctx.stroke();

  const disrupted = new Map();
  for (const [p, rid] of T.events.disrupted) disrupted.set(rid, (disrupted.get(rid) || []).concat(p));
  drawRegionBadges(T, inked, disrupted);

  if ($("showPaths").checked || selected) drawPaths(T);

  for (let y = 0; y < H; y++) {
    for (let x = 0; x < W; x++) {
      const o = trackAt(T, x, y);
      if (o < 0) continue;
      const cx = x * c + c / 2, cy = y * c + c / 2;
      ctx.strokeStyle = OWNER[o];
      ctx.lineWidth = Math.max(3, c * 0.22);
      ctx.lineCap = "round";
      let linked = false;
      for (const [dx, dy] of DIRS) {
        const nx = x + dx, ny = y + dy;
        if (nx < 0 || ny < 0 || nx >= W || ny >= H) continue;
        if (trackAt(T, nx, ny) < 0 && !townAt.has(key(nx, ny))) continue;
        ctx.beginPath();
        ctx.moveTo(cx, cy);
        ctx.lineTo(cx + dx * c / 2, cy + dy * c / 2);
        ctx.stroke();
        linked = true;
      }
      ctx.fillStyle = OWNER[o];
      ctx.beginPath();
      ctx.arc(cx, cy, Math.max(2.5, c * (linked ? 0.13 : 0.2)), 0, Math.PI * 2);
      ctx.fill();
    }
  }

  for (const [x, y, o] of T.events.placed) {
    ctx.strokeStyle = o === 2 ? "#ffffff" : OWNER[o];
    ctx.lineWidth = 2;
    ctx.strokeRect(x * c + 2, y * c + 2, c - 4, c - 4);
  }

  for (const t of replay.towns) {
    const cx = t.x * c + c / 2, cy = t.y * c + c / 2;
    ctx.fillStyle = "#f2bb13";
    ctx.strokeStyle = "#1b1b1b";
    ctx.lineWidth = 2;
    ctx.beginPath();
    ctx.rect(cx - c * 0.38, cy - c * 0.38, c * 0.76, c * 0.76);
    ctx.fill();
    ctx.stroke();
    ctx.fillStyle = "#1b1b1b";
    ctx.font = `bold ${Math.max(9, Math.floor(c * 0.45))}px sans-serif`;
    ctx.textAlign = "center";
    ctx.textBaseline = "middle";
    ctx.fillText(t.id, cx, cy + 1);
  }
}

function drawRegionBadges(T, inked, disrupted) {
  const c = cell;
  const anchor = new Map();
  for (let y = 0; y < replay.height; y++)
    for (let x = 0; x < replay.width; x++) {
      const r = replay.regions[y][x];
      if (!anchor.has(r)) anchor.set(r, [x, y]);
    }
  ctx.textAlign = "left";
  ctx.textBaseline = "top";
  ctx.font = `${Math.max(8, Math.floor(c * 0.3))}px sans-serif`;
  for (const [r, [x, y]] of anchor) {
    const inst = T.instability[r];
    const hits = disrupted.get(r);
    if (hits)
      for (const p of hits) {
        ctx.strokeStyle = OWNER[p];
        ctx.lineWidth = 3;
        ctx.strokeRect(x * c + 1.5, y * c + 1.5, c - 3, c - 3);
      }
    if (T.events.inked.includes(r)) {
      ctx.fillStyle = "#f2bb13";
      ctx.fillText("INKED", x * c + 3, y * c + 2);
      continue;
    }
    if (inked.has(r)) continue;
    const parts = [];
    if ($("showRegions").checked) parts.push("r" + r);
    if (inst > 0) parts.push("⚠" + inst);
    if (!parts.length) continue;
    ctx.fillStyle = inst >= 3 ? "#b3001b" : inst > 0 ? "#7a3d00" : "rgba(0, 0, 0, 0.45)";
    ctx.fillText(parts.join(" "), x * c + 3, y * c + 2);
  }
}

function drawPaths(T) {
  const c = cell;
  for (const conn of T.connections) {
    const id = conn.from + "-" + conn.to;
    if (selected && selected !== id) continue;
    ctx.strokeStyle = selected ? "rgba(255, 255, 255, 0.95)" : "rgba(255, 255, 255, 0.35)";
    ctx.lineWidth = selected ? c * 0.5 : c * 0.4;
    ctx.lineCap = "round";
    ctx.lineJoin = "round";
    ctx.beginPath();
    conn.path.forEach(([x, y], i) => {
      const px = x * c + c / 2, py = y * c + c / 2;
      if (i === 0) ctx.moveTo(px, py); else ctx.lineTo(px, py);
    });
    ctx.stroke();
  }
}

function hover(ev) {
  if (!replay) return;
  const rect = canvas.getBoundingClientRect();
  const x = Math.floor((ev.clientX - rect.left) / cell);
  const y = Math.floor((ev.clientY - rect.top) / cell);
  const tip = $("tip");
  if (x < 0 || y < 0 || x >= replay.width || y >= replay.height) { tip.hidden = true; return; }
  const T = replay.turns[turn];
  const r = replay.regions[y][x];
  const type = Number(replay.types[y][x]);
  const o = trackAt(T, x, y);
  const lines = [
    `(${x}, ${y})  ${TERRAIN_NAME[type]}  cost ${type + 1}`,
    `region ${r}  instability ${T.instability[r]}${T.inked.includes(r) ? "  INKED" : ""}`,
    `track ${o < 0 ? "none" : o === 2 ? "neutral" : "P" + o}`,
  ];
  const town = townAt.get(key(x, y));
  if (town) lines.push(`town ${town.id}  wants ${town.desired.length ? town.desired.join(",") : "nothing"}`);
  const through = T.connections.filter((cn) => cn.path.some(([px, py]) => px === x && py === y));
  if (through.length) lines.push("on " + through.map((cn) => cn.from + "→" + cn.to).join(" "));
  tip.textContent = lines.join("\n");
  tip.hidden = false;
  const wrap = canvas.parentElement.getBoundingClientRect();
  let left = ev.clientX - wrap.left + 14;
  if (left + tip.offsetWidth > wrap.width) left = ev.clientX - wrap.left - tip.offsetWidth - 14;
  tip.style.left = Math.max(0, left) + "px";
  tip.style.top = ev.clientY - wrap.top + 14 + "px";
}

function play() {
  if (timer) { stop(); return; }
  if (!replay) return;
  if (turn >= replay.turns.length - 1) show(0);
  $("play").textContent = "⏸";
  timer = setInterval(() => {
    if (turn >= replay.turns.length - 1) { stop(); return; }
    show(turn + 1);
  }, Number($("speed").value));
}

function stop() {
  clearInterval(timer);
  timer = null;
  $("play").textContent = "▶";
}

$("load").onclick = load;
$("replay").onchange = load;
$("play").onclick = play;
$("first").onclick = () => show(0);
$("last").onclick = () => replay && show(replay.turns.length - 1);
$("prev").onclick = () => show(turn - 1);
$("next").onclick = () => show(turn + 1);
$("scrub").oninput = (e) => show(Number(e.target.value));
$("speed").onchange = () => { if (timer) { stop(); play(); } };
$("showRegions").onchange = draw;
$("showPaths").onchange = draw;
canvas.onmousemove = hover;
canvas.onmouseleave = () => { $("tip").hidden = true; };
window.onresize = resize;

document.addEventListener("keydown", (e) => {
  if (e.target.tagName === "SELECT" || e.target.tagName === "INPUT") return;
  if (e.key === "ArrowLeft") { stop(); show(turn - 1); }
  else if (e.key === "ArrowRight") { stop(); show(turn + 1); }
  else if (e.key === " ") { e.preventDefault(); play(); }
});

const param = new URLSearchParams(location.search).get("replay");
if (param) {
  const opt = document.createElement("option");
  opt.value = param;
  opt.textContent = param.split("/").pop();
  $("replay").appendChild(opt);
  $("replay").value = param;
}
load();
