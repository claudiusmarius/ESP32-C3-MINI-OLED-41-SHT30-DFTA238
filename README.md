# ESP32-C3-MINI-OLED-41-SHT30-DFTA238
ESP32 C OLED INTEGRE SHT30

# Thermomètre connecté ESP32-C3 avec écran OLED 0.42" intégré

Ce projet repose sur le microcontrôleur **ESP32-C3** avec **écran OLED 0.42" intégré**, capable d’afficher en temps réel **l’heure synchronisée NTP** et la **température ambiante** mesurée via un **capteur SHT30**.

Il inclut une interface de **saisie Wi-Fi à trois boutons physiques**, un affichage **fenestré optimisé**, et une **gestion automatique du passage heure d’été / heure d’hiver**.

---

## ⚙️ Fonctionnalités principales

- 🕒 **Affichage horaire synchronisé NTP**
  - Changement automatique heure d’été / heure d’hiver (Europe)
- 🌡️ **Lecture de température via capteur SHT30**
  - Lecture I²C avec adresse `0x45`
- 📶 **Saisie Wi-Fi par boutons**
  - 3 boutons : UP / NEXT / OK
  - Sauvegarde SSID et mot de passe en NVS
- 💬 **Messages contextuels**
  - “OK pour config” / “NO GOOD!” / “Appui long…” / “Décompte 12s”
- 🪶 **Affichage fenestré dynamique**
  - Optimisé pour le petit écran 72x40 pixels (OLED 0.42”)
- 🧠 **Architecture modulaire**
  - Compatible avec ajout futur : DFPlayer Pro, LED WS2812, buzzer horaire, etc.

---

## 🪛 Câblage

| Élément         | Broche ESP32-C3 | Remarques |
|-----------------|------------------|------------|
| OLED intégré    | GPIO 5 (SCL), 6 (SDA) | Déjà câblé sur la carte |
| Bouton UP       | GPIO 4 | Entrée avec pull-up interne |
| Bouton NEXT     | GPIO 7 | Entrée avec pull-up interne |
| Bouton OK       | GPIO 10 | Entrée avec pull-up interne |
| Capteur SHT30   | GPIO 5 (SCL), GPIO 6 (SDA) | Adresse I²C `0x45` |
| Buzzer (option) | GPIO 8 | Bip horaire à venir |

---

## 🧩 Bibliothèques nécessaires

- `U8g2` – affichage OLED  
- `WiFi.h` – gestion Wi-Fi  
- `Preferences.h` – stockage SSID / mot de passe  
- `NTPClient.h` – synchronisation horaire  
- `Adafruit_SHT31.h` – capteur de température  
- `Wire.h` – communication I²C

---

## 🔋 Futurs ajouts prévus

- 🔊 Annonces vocales via **DFPlayer Pro**  
- 💡 Colonne de température via **LEDs WS2812**  
- ⏱️ Décompte ou animation pour les événements saisonniers (Noël, Nouvel An)

---

## 📸 Aperçu

*(à compléter par une photo du montage ou capture OLED)*

---

## 🧠 Auteur

Projet développé par **Claude Dufourmont**  
💬 “Une évolution étape par étape, sans régression, mais toujours vers la perfection !”
