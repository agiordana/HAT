-- phpMyAdmin SQL Dump
-- version 3.4.11.1deb2+deb7u2
-- http://www.phpmyadmin.net
--
-- Host: localhost
-- Generato il: Gen 18, 2016 alle 11:39
-- Versione del server: 5.5.43
-- Versione PHP: 5.4.45-0+deb7u2

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Database: `horus-ws`
--

-- --------------------------------------------------------

--
-- Struttura della tabella `mod_horus_configurations`
--

CREATE TABLE IF NOT EXISTS `mod_horus_configurations` (
  `code` varchar(255) NOT NULL,
  `value` varchar(255) NOT NULL,
  PRIMARY KEY (`code`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dump dei dati per la tabella `mod_horus_configurations`
--

INSERT INTO `mod_horus_configurations` (`code`, `value`) VALUES
('APPLICATION_ID', '70a8d59770b5b9468347d0230'),
('ENERGY_METER_POWER', '950'),
('HORUS_MODE', 'normal'),
('USER_AUTH', 'ADMIN');

-- --------------------------------------------------------

--
-- Struttura della tabella `mod_horus_ext_availables`
--

CREATE TABLE IF NOT EXISTS `mod_horus_ext_availables` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(25) NOT NULL,
  `label` varchar(20) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=6 ;

--
-- Dump dei dati per la tabella `mod_horus_ext_availables`
--

INSERT INTO `mod_horus_ext_availables` (`id`, `name`, `label`) VALUES
(1, 'Alarm_MANUAL', 'Manual'),
(2, 'Alarm_AUTO', 'Auto'),
(3, 'Alarm_ZONES', 'Zones'),
(4, 'Administration', 'Administration'),
(5, 'Alarm_OFF', 'On/Off');

-- --------------------------------------------------------

--
-- Struttura della tabella `mod_horus_users`
--

CREATE TABLE IF NOT EXISTS `mod_horus_users` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(25) NOT NULL,
  `email` varchar(64) DEFAULT NULL,
  `password` varchar(50) NOT NULL,
  `enable` tinyint(1) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=4 ;

--
-- Dump dei dati per la tabella `mod_horus_users`
--

INSERT INTO `mod_horus_users` (`id`, `name`, `email`, `password`, `enable`) VALUES
(1, 'ADMIN', NULL, '123456', 1),
(3, 'Dino', NULL, '12345678', 1);

-- --------------------------------------------------------

--
-- Struttura della tabella `mod_horus_user_has_ext_availables`
--

CREATE TABLE IF NOT EXISTS `mod_horus_user_has_ext_availables` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `mod_horus_user_id` int(11) NOT NULL,
  `mod_horus_ext_available_id` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=13 ;

--
-- Dump dei dati per la tabella `mod_horus_user_has_ext_availables`
--

INSERT INTO `mod_horus_user_has_ext_availables` (`id`, `mod_horus_user_id`, `mod_horus_ext_available_id`) VALUES
(1, 1, 1),
(2, 1, 2),
(3, 1, 3),
(4, 1, 4),
(5, 1, 5),
(12, 3, 4);

DELIMITER $$
--
-- Eventi
--
CREATE DEFINER=`root`@`localhost` EVENT `delete_log_expired` ON SCHEDULE EVERY 1 DAY STARTS '2014-10-23 16:28:08' ON COMPLETION NOT PRESERVE ENABLE DO DELETE FROM mod_horus_logs WHERE time < CURRENT_TIMESTAMP - INTERVAL 30 DAY$$

DELIMITER ;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
