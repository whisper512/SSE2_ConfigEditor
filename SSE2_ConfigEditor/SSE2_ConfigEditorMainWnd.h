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

enum eFaction
{
    Faction_TL,
    Faction_TR,
    Faction_VL,
    Faction_VR,
    Faction_AL,
    Faction_AR
};

enum eCapitalship
{
    Capitalship_battle,  //战斗主力舰
    Capitalship_colony,  //殖民主力舰
    Capitalship_support, //支援主力舰
    Capitalship_carrier, //航母主力舰
    Capitalship_siege,   //攻城主力舰
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
    eCapitalship eCapitalship;              //主力舰类型
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

    void ConnectSlots();
    
    void ReadConfig();

    void ParseUnitLimitConfigFromJson(const QJsonDocument& jsonDoc);
    void UpdateUnitsLimitData();
    void WriteUnitLimitConfigToJson();

    void ParseMaxSupplyConfigFromJson(const QJsonDocument& jsonDoc);
    void UpdateMaxSupplyData();
    void WriteMaxSupplyConfigToJson();

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

