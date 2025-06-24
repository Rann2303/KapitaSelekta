<?php
require 'db_config.php';
header('Content-Type: application/json');

$action = $_GET['action'] ?? '';

if ($action == 'masuk') {
    $jenis = $_POST['jenis'] ?? 'mobil';

    // Catat kendaraan masuk
    $query = "INSERT INTO kendaraan (jenis, status, waktu) VALUES ('$jenis', 'masuk', NOW())";
    if (mysqli_query($conn, $query)) {
        $kendaraan_id = mysqli_insert_id($conn);

        // Simpan waktu masuk ke tabel transaksi (sementara, waktu_keluar kosong)
        $transaksi = mysqli_query($conn, "
            INSERT INTO transaksi (kendaraan_id, waktu_masuk)
            VALUES ($kendaraan_id, NOW())
        ");

        echo json_encode(["status" => "success", "id" => $kendaraan_id]);
    } else {
        echo json_encode(["status" => "error", "message" => mysqli_error($conn)]);
    }

} elseif ($action == 'keluar') {
    $jenis = $_POST['jenis'] ?? 'mobil';

    // Tambahkan data kendaraan keluar
    $query = "INSERT INTO kendaraan (jenis, status, waktu) VALUES ('$jenis', 'keluar', NOW())";
    if (mysqli_query($conn, $query)) {
        $kendaraan_keluar_id = mysqli_insert_id($conn);

        // Cari transaksi terakhir kendaraan yang masuk dan belum keluar
        $find = mysqli_query($conn, "
            SELECT * FROM transaksi
            WHERE waktu_keluar IS NULL
            ORDER BY waktu_masuk DESC
            LIMIT 1
        ");

        if (mysqli_num_rows($find) > 0) {
            $row = mysqli_fetch_assoc($find);
            $id_transaksi = $row['id'];
            $waktu_masuk = strtotime($row['waktu_masuk']);
            $waktu_keluar = time();
            $durasi_menit = round(($waktu_keluar - $waktu_masuk) / 60);

            // Update transaksi dengan waktu_keluar dan durasi
            $update = mysqli_query($conn, "
                UPDATE transaksi
                SET waktu_keluar = NOW(), durasi_menit = $durasi_menit
                WHERE id = $id_transaksi
            ");
        }

        echo json_encode(["status" => "success", "id" => $kendaraan_keluar_id]);
    } else {
        echo json_encode(["status" => "error", "message" => mysqli_error($conn)]);
    }

} elseif ($action == 'truk_ditolak') {
    $insert = mysqli_query($conn, "
        INSERT INTO kendaraan (jenis, status, waktu)
        VALUES ('truk', 'ditolak', NOW())
    ");
    echo json_encode(["status" => "success"]);

} elseif ($action == 'slot') {
    $slots = json_decode($_POST['data'], true);

    foreach ($slots as $i => $status) {
        $nomor = $i + 1;
        $terisi = $status ? 1 : 0;

        $cek = mysqli_query($conn, "SELECT * FROM slot WHERE slot_nomor=$nomor");
        if (mysqli_num_rows($cek) > 0) {
            mysqli_query($conn, "UPDATE slot SET terisi=$terisi WHERE slot_nomor=$nomor");
        } else {
            mysqli_query($conn, "INSERT INTO slot (slot_nomor, terisi) VALUES ($nomor, $terisi)");
        }
    }

    echo json_encode(["status" => "success"]);

} else {
    echo json_encode(["status" => "error", "message" => "Aksi tidak dikenali"]);
}
?>
