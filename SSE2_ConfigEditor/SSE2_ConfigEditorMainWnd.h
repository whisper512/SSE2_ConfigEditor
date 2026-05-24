#pragma once
#include <QtWidgets/QMainWindow>
#include <QAction>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "ui_SSE2_ConfigEditorMainWnd.h"

#define TL_PlayerConfig "trader_loyalist"
#define TR_PlayerConfig "trader_rebel"
#define VL_PlayerConfig "vasari_loyalist"
#define VR_PlayerConfig "vasari_rebel"
#define AL_PlayerConfig "advent_loyalist"
#define AR_PlayerConfig "advent_rebel"

#define Advent_BattleCaptialShip "advent_battle_capital_ship" // 光辉
#define Advent_ColonyCaptialShip "advent_colony_capital_ship" // 先祖
#define Advent_CarrierCaptialShip "advent_carrier_capital_ship" // 宁静
#define Advent_BattlePsionicCapitalShip "advent_battle_psionic_capital_ship" // 超度
#define Advent_PlanetPsionicCaptialShip "advent_planet_psionic_capital_ship" // 天启

#define Trader_BattleCaptialShip "Trader_battle_capital_ship" // 科尔
#define Trader_ColonyCaptialShip "Trader_colony_capital_ship" // 艾肯
#define Trader_CarrierCaptialShip "Trader_carrier_capital_ship" // 瓦索
#define Trader_SiegeCaptialShip "Trader_siege_capital_ship" // 玛莎
#define Trader_SupportCaptialShip "Trader_support_capital_ship" // 杜诺夫

#define Vasari_BattleCaptialShip "vasari_capital_ship" // 克尔图
#define Vasari_ColonyCaptialShip "vasari_colony_capital_ship" // 加拉苏
#define Vasari_CarrierCaptialShip "vasari_carrier_capital_ship" // 基兰查
#define Vasari_SiegeCaptialShip "vasari_siege_capital_ship" // 瓦诺克斯
#define Vasari_MarauderCaptialShip "vasari_marauder_capital_ship" // 安特可


enum eFaction
{
    Faction_TL,
    Faction_TR,
    Faction_VL,
    Faction_VR,
    Faction_AL,
    Faction_AR
};

// A:光辉 B:先祖 C:宁静 D:超度 E:天启
// A:科尔 B:艾肯 C:瓦索 D:玛莎 E:杜诺夫
// A:克尔图 B:加拉苏 C:基兰查 D:瓦诺克斯 E:安特可
enum eCapitalshipIndex
{
    Capitalship_A, 
    Capitalship_B,
    Capitalship_C,
    Capitalship_D,
    Capitalship_E
};

struct stuCapitalshipLevelInfo
{
    double MaxHull;
    double HullRestoreRate;
    double HullRestoreCooldown;
    double HullRestoreScale;
    double HullCrippledPercentage;
    double MaxArmor;
    double ArmorRestoreRate;
    double ArmorRestoreCooldown;
    double ArmorRestoreScale;
    double ArmorStrength;
    double MaxShield;
    double ShieldRestoreRate;
    double ShieldRestoreCooldown;
    double ShieldRestoreScale;
    
    stuCapitalshipLevelInfo()
    {
        MaxHull = 0.0;
        HullRestoreRate = 0.0;
        HullRestoreCooldown = 0.0;
        HullRestoreScale = 0.0;
        HullCrippledPercentage = 0.0;
        MaxArmor = 0.0;
        ArmorRestoreRate = 0.0;
        ArmorRestoreCooldown = 0.0;
        ArmorRestoreScale = 0.0;
        ArmorStrength = 0.0;
        MaxShield = 0.0;
        ShieldRestoreRate = 0.0;
        ShieldRestoreCooldown = 0.0;
        ShieldRestoreScale = 0.0;
    }
};

struct stuCapitalshipInfo
{
    eCapitalshipIndex eCapitalship;         //主力舰类型
    stuCapitalshipLevelInfo LevelInfo[10];  //等级信息
};


class SSE2_ConfigEditorMainWnd : public QMainWindow
{
    Q_OBJECT

public:
    SSE2_ConfigEditorMainWnd(QWidget *parent = nullptr);
    ~SSE2_ConfigEditorMainWnd();

private:
    Ui::SSE2_ConfigEditorMainWndClass ui;

private:
    QAction *m_pActionOpenGamePath;
    QAction *m_pActionWriteConfig;
    QAction *m_pActionSaveBackup;
    
    QString m_strGamePath;       //游戏路径
    eFaction m_eFaction;         //阵营
    int m_iTitanNum;             //泰坦数量
    int m_iSuperCapitalshipNum;  //超级主力舰数量
    int m_istarStarbase;         //恒星星际基地数量
    int m_iplanetStarbase;       //行星星际基地数量
    QList<int> m_listMaxSupply;  // 存储各级max_supply值
    int m_iDefaultCredits;       // 默认起始信用点
    int m_iDefaultMetal;         // 默认起始金属
    int m_iDefaultCrystal;       // 默认起始水晶
     
    stuCapitalshipInfo m_listTradeCapitalshipInfo[5];  //T族主力舰
    stuCapitalshipInfo m_listVasariCapitalshipInfo[5];  //V族主力舰
    stuCapitalshipInfo m_listAdventCapitalshipInfo[5];  //A族主力舰


private:
    void InitApplication();
    void InitMainWnd();
    void IntiEditor();
    void InitMenu();
    void InitData();

    void refreshCapitalshipCombox();

    void ReadConfig();
    void ConnectSlots();

    // 单位限制相关
    void ParseUnitLimitConfigFromJson(const QJsonDocument& jsonDoc);
    void UpdateUnitsLimitData();
    void WriteUnitLimitConfigToJson();

    // 最大供应相关
    void ParseMaxSupplyConfigFromJson(const QJsonDocument& jsonDoc);
    void UpdateMaxSupplyData();
    void WriteMaxSupplyConfigToJson();

    // 默认起始资产相关
    void ParseDefaultStartingAssetsFromJson(const QJsonDocument& jsonDoc);
    void UpdateDefaultStartingAssetsData();
    void WriteDefaultStartingAssetsToJson();



    void OnFactionChanged(int index);
    void OnOpenGamePath();
    void OnEditConfig();
    void OnSaveBackup();

private slots:
    void OnEditFinished();

};

