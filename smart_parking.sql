-- 1. Buat database jika belum ada
CREATE DATABASE IF NOT EXISTS smart_parking;
USE smart_parking;

-- 2. Tabel kendaraan
CREATE TABLE IF NOT EXISTS kendaraan (
    id INT AUTO_INCREMENT PRIMARY KEY,
    jenis ENUM('mobil', 'truk') NOT NULL,
    status ENUM('masuk', 'keluar', 'ditolak') NOT NULL,
    waktu TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- 3. Tabel slot parkir
CREATE TABLE IF NOT EXISTS slot (
    id INT AUTO_INCREMENT PRIMARY KEY,
    slot_nomor INT NOT NULL,
    terisi BOOLEAN NOT NULL,
    waktu_update TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
);

-- 4. Tabel transaksi
CREATE TABLE IF NOT EXISTS transaksi (
    id INT AUTO_INCREMENT PRIMARY KEY,
    kendaraan_id INT,
    jumlah INT DEFAULT 3000,
    waktu_masuk TIMESTAMP NULL,
    waktu_keluar TIMESTAMP NULL,
    durasi_menit INT DEFAULT 0,
    FOREIGN KEY (kendaraan_id) REFERENCES kendaraan(id)
);

-- 5. Trigger untuk menghitung durasi_menit saat UPDATE waktu_keluar
DELIMITER //

CREATE TRIGGER hitung_durasi_sebelum_update
BEFORE UPDATE ON transaksi
FOR EACH ROW
BEGIN
    IF NEW.waktu_masuk IS NOT NULL AND NEW.waktu_keluar IS NOT NULL THEN
        SET NEW.durasi_menit = GREATEST(0, TIMESTAMPDIFF(MINUTE, NEW.waktu_masuk, NEW.waktu_keluar));
    END IF;
END;
//

DELIMITER ;
