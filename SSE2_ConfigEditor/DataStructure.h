#pragma once

#include <QString>
#include <QStringList>
#include <QVector>
#include <QMap>

// ============================================
// 阵营 .player 文件名字符串宏
// ============================================
#define TL_PlayerConfig "trader_loyalist"
#define TR_PlayerConfig "trader_rebel"
#define VL_PlayerConfig "vasari_loyalist"
#define VR_PlayerConfig "vasari_rebel"
#define AL_PlayerConfig "advent_loyalist"
#define AR_PlayerConfig "advent_rebel"

// ============================================
// 主力舰 .unit 文件名字符串宏
// ============================================
#define Advent_BattleCaptialShip        "advent_battle_capital_ship"
#define Advent_ColonyCaptialShip        "advent_colony_capital_ship"
#define Advent_CarrierCaptialShip       "advent_carrier_capital_ship"
#define Advent_BattlePsionicCapitalShip "advent_battle_psionic_capital_ship"
#define Advent_PlanetPsionicCaptialShip "advent_planet_psionic_capital_ship"

#define Trader_BattleCaptialShip   "Trader_battle_capital_ship"
#define Trader_ColonyCaptialShip   "Trader_colony_capital_ship"
#define Trader_CarrierCaptialShip  "Trader_carrier_capital_ship"
#define Trader_SiegeCaptialShip    "Trader_siege_capital_ship"
#define Trader_SupportCaptialShip  "Trader_support_capital_ship"

#define Vasari_BattleCaptialShip   "vasari_battle_capital_ship"
#define Vasari_ColonyCaptialShip   "vasari_colony_capital_ship"
#define Vasari_CarrierCaptialShip  "vasari_carrier_capital_ship"
#define Vasari_SiegeCaptialShip    "vasari_siege_capital_ship"
#define Vasari_MarauderCaptialShip "vasari_marauder_capital_ship"

// ============================================
// 阵营枚举
// ============================================
enum eFaction
{
    Faction_TL,
    Faction_TR,
    Faction_VL,
    Faction_VR,
    Faction_AL,
    Faction_AR,
    Faction_Count
};

// ============================================
// 主力舰索引枚举
// ============================================
enum eCapitalshipIndex
{
    Capitalship_A,
    Capitalship_B,
    Capitalship_C,
    Capitalship_D,
    Capitalship_E
};

// ============================================
// 主力舰单等级数据
// ============================================
struct stuCapitalshipLevelInfo
{
    double MaxHull = 0.0;
    double HullRestoreRate = 0.0;
    double HullRestoreCooldown = 0.0;
    double HullRestoreScale = 0.0;
    double HullCrippledPercentage = 0.0;
    double MaxArmor = 0.0;
    double ArmorRestoreRate = 0.0;
    double ArmorRestoreCooldown = 0.0;
    double ArmorRestoreScale = 0.0;
    double ArmorStrength = 0.0;
    double MaxShield = 0.0;
    double ShieldRestoreRate = 0.0;
    double ShieldRestoreCooldown = 0.0;
    double ShieldRestoreScale = 0.0;
};

// ============================================
// 主力舰结构体
// ============================================
struct stuCapitalshipInfo
{
    eCapitalshipIndex eCapitalship = Capitalship_A;
    stuCapitalshipLevelInfo LevelInfo[10];
};

// ============================================
// 单位限制配置
// ============================================
struct FactionUnitLimits
{
    int titan = 1;
    int superCapitalShip = 1;
    int starStarbase = 4;
    int planetStarbase = 1;
};

// ============================================
// 起始资产配置
// ============================================
struct FactionStartingAssets
{
    int credits = 1000;
    int metal = 400;
    int crystal = 250;
    bool hasCredits = true;   // 瓦萨里阵营没有 credits 字段
};

// ============================================
// 单个阵营的全部可编辑数据
// ============================================
struct FactionData
{
    eFaction      faction;
    QString       displayName;
    QString       playerFileName;              // 无 .player 后缀
    QStringList   capitalShipFileNames;        // 5个 .unit 文件名
    QStringList   capitalShipDisplayNames;     // 5个中文名

    FactionUnitLimits      unitLimits;
    QVector<int>           maxSupplyLevels;
    FactionStartingAssets  startingAssets;
    QVector<stuCapitalshipInfo> capitalShips;

    // 辅助
    bool hasCredits()       const { return startingAssets.hasCredits; }
    int  supplyLevelCount() const { return maxSupplyLevels.size(); }

    FactionData()
        : faction(Faction_TL)
    {
        capitalShips.resize(5);
        // 直接使用枚举常量，避免 static_cast 可能引起的编译器警告
        capitalShips[0].eCapitalship = Capitalship_A;
        capitalShips[1].eCapitalship = Capitalship_B;
        capitalShips[2].eCapitalship = Capitalship_C;
        capitalShips[3].eCapitalship = Capitalship_D;
        capitalShips[4].eCapitalship = Capitalship_E;
    }
};

// ============================================
// 生成 6 个阵营的默认配置（不含 JSON 数据，仅文件名/显示名等）
// 实现放在对应的 .cpp 文件中
// ============================================
QMap<eFaction, FactionData> CreateDefaultFactionConfigs();
