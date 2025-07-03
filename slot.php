<?php
require 'db_config.php';

$result = mysqli_query($conn, "SELECT * FROM slot ORDER BY slot_nomor ASC");
$slots = [];
while ($row = mysqli_fetch_assoc($result)) {
    $slots[] = $row;
}
?>
<!DOCTYPE html>
<html>
<head>
    <meta http-equiv="refresh" content="5">
    <title>Status Slot Parkir</title>
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/css/bootstrap.min.css" rel="stylesheet">
    <style>
        body {
            font-family: 'Segoe UI', sans-serif;
            background-color: #ecf8f8;
            margin: 0;
            padding: 20px;
            color: #2c3e50;
        }
        h2 {
            text-align: center;
            margin-top: 20px;
            font-weight: bold;
            color: #2c3e50;
        }
        .slot-container {
            display: flex;
            justify-content: center;
            flex-wrap: wrap;
            gap: 20px;
            margin-top: 30px;
        }
        .slot-box {
            width: 180px;
            height: 100px;
            border-radius: 16px;
            display: flex;
            flex-direction: column;
            justify-content: center;
            align-items: center;
            color: white;
            font-size: 20px;
            font-weight: bold;
            box-shadow: 0 6px 15px rgba(0,0,0,0.1);
            transform: scale(0.9);
            opacity: 0;
            animation: fadeIn 0.6s ease-in-out forwards;
        }
        .terisi {
            background-color: #e74c3c; /* merah */
        }
        .kosong {
            background-color: #1abc9c; /* toska */
        }
        @keyframes fadeIn {
            from {
                transform: scale(0.9);
                opacity: 0;
            }
            to {
                transform: scale(1);
                opacity: 1;
            }
        }
    </style>
</head>
<body>
    <h2>🅿️ Status Slot Parkir</h2>
    <div class="slot-container">
        <?php foreach ($slots as $slot): ?>
            <div class="slot-box <?= $slot['terisi'] ? 'terisi' : 'kosong' ?>">
                Slot <?= $slot['slot_nomor'] ?><br>
                <?= $slot['terisi'] ? '❌ Terisi' : '✅ Kosong' ?>
            </div>
        <?php endforeach; ?>
    </div>
</body>
</html>
