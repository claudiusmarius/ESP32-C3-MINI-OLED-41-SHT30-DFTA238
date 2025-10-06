# ESP32-C3-MINI-OLED-41-SHT30-DFTA238
ESP32 C OLED INTEGRE SHT30

================================================================
   🌡️  Projet : Thermomètre connecté - ESP32-C3 OLED 0.42"
   🔧  Auteur  : Claude Dufourmont
   📅  Version : 1.3 - Octobre 2025
   ================================================================

   🧭  DESCRIPTION GÉNÉRALE
   ---------------------------------------------------------------
   Programme destiné à un module ESP32-C3 équipé d’un écran OLED
   intégré (0.42"), gérant la saisie et la mémorisation du SSID et
   du mot de passe WiFi, l’affichage de l’adresse IP locale, ainsi
   que la synchronisation automatique de l’heure via NTP.

   🖱️  INTERFACE UTILISATEUR
   ---------------------------------------------------------------
   Trois boutons physiques permettent la navigation :
     - UP   → GPIO 4  : fait défiler les caractères
     - NEXT → GPIO 7  : change de ligne de caractères (min/maj/symb)
     - OK   → GPIO 10 : sélectionne ou valide (appui long = config)

   ✨  PRINCIPALES FONCTIONNALITÉS
   ---------------------------------------------------------------
   • Saisie directe du SSID et du mot de passe WiFi
   • Affichage fenestré dynamique (12 derniers caractères visibles)
   • Stockage permanent en mémoire NVS (librairie Preferences)
   • Connexion WiFi automatique au démarrage
   • Affichage de l’adresse IP locale (partiellement masquée)
   • Synchronisation horaire NTP
   • Gestion automatique du passage heure d’été / heure d’hiver
   • Animation de barre de progression lors de l’invitation d’appui long

   🧩  NOTES TECHNIQUES
   ---------------------------------------------------------------
   - Écran OLED : U8G2_SSD1306_72X40_ER_F_HW_I2C
   - Connexion I2C :
        SDA → GPIO 5
        SCL → GPIO 6
   - Compatible avec les bibliothèques :
        <WiFi.h>, <Preferences.h>, <U8g2lib.h>, <NTPClient.h>, <WiFiUdp.h>
   - Optimisé pour l’ESP32-C3 Mini avec écran intégré 0.42"

   ⚙️  VERSION ACTUELLE
   ---------------------------------------------------------------
   - Fenêtre de saisie améliorée (affichage non tronqué)
   - Décompte d’invitation lisible et distinct de la barre
   - Base solide pour intégration future :
        > Capteur SHT30
        > Bandeau de LEDs adressables
        > Module DFPlayer Pro (annonces vocales)

   ---------------------------------------------------------------
   © Claude Dufourmont - 2025
   Code libre pour usage personnel et expérimental
   ================================================================
