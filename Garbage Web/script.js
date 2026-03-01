import { initializeApp } from "https://www.gstatic.com/firebasejs/10.12.5/firebase-app.js";
import { getDatabase, ref, onValue } from "https://www.gstatic.com/firebasejs/10.12.5/firebase-database.js";

// Firebase config
const firebaseConfig = {
  apiKey: "AIzaSyAoE0KT_XObQDO6HHMvfuN13kImWg17HfE",
  authDomain: "waste-collection-robot-85d70.firebaseapp.com",
  databaseURL: "https://waste-collection-robot-85d70-default-rtdb.asia-southeast1.firebasedatabase.app",
  projectId: "waste-collection-robot-85d70",
  storageBucket: "waste-collection-robot-85d70.firebasestorage.app",
  messagingSenderId: "112532754158",
  appId: "1:112532754158:web:e96f93826c3fd53b3aadff"
};

const app = initializeApp(firebaseConfig);
const db = getDatabase(app);

// Firebase refs
const levelRef = ref(db, "dustbin/garbageLevel");
const lidRef = ref(db, "dustbin/lidStatus");
const stateRef = ref(db, "robot/state");
const targetRef = ref(db, "robot/target");
const obstacleRef = ref(db, "robot/obstacleDistance");
const historyRef = ref(db, "dustbin/history");

// DOM elements
const levelText = document.getElementById("levelText");
const progressCircle = document.querySelector(".circular-progress circle.progress");
const statusEl = document.getElementById("status");
const lidEl = document.getElementById("lidStatus");
const lidIcon = document.getElementById("lidIcon");
const robotStateEl = document.getElementById("robotState");
const robotTargetEl = document.getElementById("robotTarget");
const robotObstacleEl = document.getElementById("robotObstacle");
const chartContainer = document.getElementById("historyChart");

// ---------------- Garbage Level ----------------
onValue(levelRef, snapshot => {
  if (!snapshot.exists()) return;
  const level = snapshot.val();
  levelText.innerText = level + "%";
  const dashoffset = 400 - (400 * level / 100);
  progressCircle.style.strokeDashoffset = dashoffset;

  let color, statusText;
  if (level < 30) { color = "#34d399"; statusText = "EMPTY"; }
  else if (level < 80) { color = "#fbbf24"; statusText = "HALF"; }
  else { color = "#ef4444"; statusText = "FULL"; }

  progressCircle.style.stroke = color;
  statusEl.innerText = statusText;
  statusEl.className = `status ${statusText}`;
});

// ---------------- Lid ----------------
onValue(lidRef, snapshot => {
  if (!snapshot.exists()) return;
  const status = snapshot.val();
  lidEl.innerText = status;
  lidEl.className = `status ${status}`;
  lidIcon.className = `lid-icon ${status}`;
});

// ---------------- Robot ----------------
onValue(stateRef, snapshot => {
  if (!snapshot.exists()) return;
  const state = snapshot.val();
  robotStateEl.innerText = state;
  robotStateEl.className = `status ${state}`;
});

onValue(targetRef, snapshot => {
  if (!snapshot.exists()) return;
  const target = snapshot.val();
  robotTargetEl.innerText = target;
  robotTargetEl.className = `status target${target}`;
});

onValue(obstacleRef, snapshot => {
  if (!snapshot.exists()) return;
  const dist = snapshot.val();
  robotObstacleEl.innerText = dist;

  let colorClass = dist > 30 ? "obstacleSafe" : dist > 15 ? "obstacleCaution" : "obstacleDanger";
  robotObstacleEl.className = `status ${colorClass}`;
});

// ---------------- Chart.js ----------------
let myChart;
onValue(historyRef, snapshot => {
  const history = snapshot.val();
  if (!history) {
    chartContainer.innerHTML = "<p style='text-align:center; padding:2rem; opacity:0.7;'>No history data yet...</p>";
    return;
  }

  const chartData = Object.values(history)
    .sort((a, b) => a.time - b.time)
    .slice(-30);

  const labels = chartData.map(item => {
    const d = new Date(item.time);
    return d.toLocaleTimeString([], { hour: '2-digit', minute: '2-digit' });
  });
  const data = chartData.map(item => item.level);

  if (!myChart) {
    const canvas = document.createElement("canvas");
    chartContainer.innerHTML = "";
    chartContainer.appendChild(canvas);

    myChart = new Chart(canvas, {
      type: "line",
      data: {
        labels: labels,
        datasets: [{
          label: "Garbage Level (%)",
          data: data,
          borderColor: "#a78bfa",
          backgroundColor: "rgba(167,139,250,0.2)",
          fill: true,
          tension: 0.4,
          pointRadius: 5
        }]
      },
      options: {
        responsive: true,
        maintainAspectRatio: false,
        scales: {
          y: { beginAtZero: true, max: 100 },
          x: { ticks: { color: "#cbd5e1" } }
        },
        plugins: {
          legend: { labels: { color: "white" } },
          tooltip: { 
            backgroundColor: "rgba(30,41,59,0.95)", 
            titleColor: "white", 
            bodyColor: "white", 
            borderRadius: 12 
          }
        }
      }
    });
  } else {
    myChart.data.labels = labels;
    myChart.data.datasets[0].data = data;
    myChart.update();
  }
});
