-- Buat database jika belum ada
CREATE DATABASE IF NOT EXISTS smart_parking;
USE smart_parking;

-- Tabel kendaraan
CREATE TABLE IF NOT EXISTS kendaraan (
    id INT AUTO_INCREMENT PRIMARY KEY,
    jenis ENUM('mobil', 'truk') NOT NULL,
    status ENUM('masuk', 'keluar', 'ditolak') NOT NULL,
    waktu TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Tabel slot parkir
CREATE TABLE IF NOT EXISTS slot (
    id INT AUTO_INCREMENT PRIMARY KEY,
    slot_nomor INT NOT NULL,
    terisi BOOLEAN NOT NULL,
    waktu_update TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
);

-- Tabel transaksi
CREATE TABLE IF NOT EXISTS transaksi (
    id INT AUTO_INCREMENT PRIMARY KEY,
    kendaraan_id INT,
    jumlah INT DEFAULT 3000,
    waktu_masuk TIMESTAMP NULL,
    waktu_keluar TIMESTAMP NULL,
    durasi_menit INT DEFAULT 0,
    FOREIGN KEY (kendaraan_id) REFERENCES kendaraan(id)
);
