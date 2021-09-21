-- phpMyAdmin SQL Dump
-- version 4.9.1
-- https://www.phpmyadmin.net/
--
-- Anamakine: localhost
-- Üretim Zamanı: 21 Eyl 2021, 22:39:31
-- Sunucu sürümü: 8.0.17
-- PHP Sürümü: 7.3.10

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET AUTOCOMMIT = 0;
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Veritabanı: `banka`
--

-- --------------------------------------------------------

--
-- Tablo için tablo yapısı `customers`
--

CREATE TABLE `customers` (
  `CustomerID` int(11) NOT NULL,
  `username` varchar(255) NOT NULL,
  `password` varchar(255) NOT NULL,
  `name` varchar(255) NOT NULL,
  `bank` varchar(255) NOT NULL,
  `balance` double NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

--
-- Tablo döküm verisi `customers`
--

INSERT INTO `customers` (`CustomerID`, `username`, `password`, `name`, `bank`, `balance`) VALUES
(1, 'furkan', '1234', 'Furkan Türkay', 'A', 730),
(2, 'ahmet', '1234', 'Ahmet Yavuz', 'B', 30000),
(3, 'Mehmet', '1234', 'Mehmet Aydın', 'A', 999999),
(4, 'Faruk', '1234', 'Faruk Çetin', 'C', 4000),
(5, 'Osman', '1234', 'Osman Demir', 'C', 715);

--
-- Dökümü yapılmış tablolar için indeksler
--

--
-- Tablo için indeksler `customers`
--
ALTER TABLE `customers`
  ADD UNIQUE KEY `CustormerID` (`CustomerID`,`username`);
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
