# Station Météo STM32F746G-DISCOVERY


## Description du Projet
Cette station météo embarquée a été développée dans le contexte du réchauffement climatique, où le suivi précis des conditions météorologiques locales devient de plus en plus crucial. Basée sur la carte STM32F746G-DISCOVERY, elle combine des capteurs environnementaux de haute précision avec une interface utilisateur intuitive pour offrir une solution complète de surveillance météorologique.

### Objectifs
Cette station météo permet :
- La surveillance en temps réel des conditions atmosphériques locales
- Le stockage et l'analyse des données météorologiques
- Une visualisation claire et interactive des mesures
- Une gestion optimisée de l'énergie

## Caractéristiques Techniques

### Mesures Environnementales
- **Température** : -40°C à +120°C (capteur HTS221)
- **Humidité** : 0-100% RH (capteur HTS221)
- **Pression atmosphérique** : 260-1260 hPa (capteur LPS22HH)
- **Vent** :
  - Vitesse : Anémomètre à coupelles (2.4 km/h = 1Hz)
  - Direction : Girouette 16 positions (0-360°)
- **Pluviométrie** : Système à auget basculeur (0.2794mm/impulsion)

### Interface Utilisateur
1. **Écran Principal**
   - Affichage simultané de toutes les mesures
   - Mise à jour en temps réel
   - Navigation tactile intuitive

2. **Pages Détaillées**
   - Visualisation approfondie par type de mesure
   - Graphiques d'évolution temporelle
   - Statistiques et tendances

3. **Configuration Système**
   - Réglage date/heure
   - Paramètres d'acquisition
   - Options d'affichage

### Stockage et Communication
- Enregistrement sur carte microSD
- Format de données structuré pour analyse ultérieure
- Horodatage précis des mesures

## Prérequis Matériels

### Composants Principaux
1. **Carte STM32F746G-DISCOVERY**
   - Microcontrôleur STM32F746NGH6
   - Écran LCD 4.3" 480x272 pixels
   - Tactile capacitif
   - Support carte microSD

2. **Shield Capteurs X-NUCLEO-IKS01A3**
   - HTS221 : Température et humidité
   - LPS22HH : Pression atmosphérique
   - Communication I2C

3. **Weather Meters Sparkfun**
   - Anémomètre à coupelles
   - Girouette 16 positions
   - Pluviomètre à auget basculeur

### Connexions
```
STM32F746G-DISCOVERY
├── I2C1 (PB8/PB9)
│   └── Shield X-NUCLEO-IKS01A3
├── GPIO
│   ├── PA8  : Anémomètre
│   ├── PA15 : Pluviomètre
│   └── ADC  : Girouette
└── SDMMC
    └── Carte microSD
```

## Installation et Configuration

### Prérequis Logiciels
- STM32CubeIDE 1.9.0 ou supérieur
- STM32CubeMX intégré
- Middlewares : FatFS


2. Ouvrir le projet dans STM32CubeIDE
3. Compiler et flasher la carte

### Structure du Projet
```
station-meteo-stm32/
├── Core/
│   ├── Inc/
│   │   ├── Interface.h              # Gestion interface utilisateur
│   │   ├── Nucleo_HumTemp_I2C.h    # Driver capteurs température/humidité
│   │   ├── Nucleo_Pression_I2C.h   # Driver capteur pression
│   │   └── main.h
│   └── Src/
│       ├── Interface.c              # Implémentation interface
│       ├── Nucleo_HumTemp_I2C.c    # Gestion capteurs I2C
│       ├── Nucleo_Pression_I2C.c   # Gestion pression
│       └── main.c
├── Drivers/
│   ├── STM32F7xx_HAL_Driver/       # Drivers HAL
│   ├── BSP/                        # Support carte
│   └── CMSIS/                      # CMSIS
├── Middlewares/
│   └── Third_Party/
│       └── FatFs/                  # Système de fichiers FAT
├── Documentation/
│   ├── Images/                     # Screenshots et schémas
│   └── Rapport
└── README.md                       # Ce fichier
```

## Utilisation

### Interface Principale
L'écran d'accueil présente :
- Les mesures actuelles des différents capteurs
- L'heure et la date
- Les boutons de navigation

### Navigation
- **Appui simple** : Accès aux pages détaillées
- **Glissement** : Navigation entre les graphiques
- **Double appui** : Retour au menu principal

### Configuration
1. Accéder au menu Configuration
2. Régler date et heure
3. Configurer les paramètres d'acquisition
4. Valider les modifications

