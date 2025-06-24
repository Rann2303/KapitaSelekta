<?php // dashboard.php ?>
<?php include 'db_config.php'; ?>
<!DOCTYPE html>
<html>
<head>
    <title>Dashboard Smart Parking</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #e9f5f3;
            margin: 0;
            padding: 0;
        }
        header {
            background-color: #1c3c57;
            color: white;
            padding: 20px;
            text-align: center;
        }
        .content {
            padding: 20px;
            background-color: white;
            min-height: 400px;
            animation: fadeIn 0.5s ease-in-out;
        }
        @keyframes fadeIn {
            from { opacity: 0; transform: translateY(10px); }
            to { opacity: 1; transform: translateY(0); }
        }
    </style>
</head>
<body>

<header>
    <h1>Dashboard Smart Parking</h1>
    <p>Monitoring Status Slot Parkir Real-Time</p>
</header>

<div id="mainContent" class="content">
    <?php include 'slot.php'; ?>
</div>

<script>
function loadSlot() {
    fetch('slot.php')
        .then(response => response.text())
        .then(data => {
            document.getElementById('mainContent').innerHTML = data;
        });
}

// Muat awal dan auto-refresh setiap 5 detik
loadSlot();
setInterval(loadSlot, 5000);
</script>

</body>
</html>
