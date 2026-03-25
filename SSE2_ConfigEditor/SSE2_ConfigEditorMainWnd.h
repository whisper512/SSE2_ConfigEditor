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

enum Faction
{
    Faction_TL,
    Faction_TR,
    Faction_VL,
    Faction_VR,
    Faction_AL,
    Faction_AR
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
    
    QString m_strGamePath;
    
    Faction m_eFaction;
    int m_iTitanNum;
    int m_iSuperCapitalshipNum;
    int m_istarStarbase;
    int m_iplanetStarbase;
    QList<int> m_listMaxSupply;  // 存储各级max_supply 值
    int m_iDefaultCredits;      // 默认起始信用点
    int m_iDefaultMetal;        // 默认起始金属
    int m_iDefaultCrystal;      // 默认起始水晶

private:
    void InitApplication();
    void InitMainWnd();
    void IntiEditor();
    void InitMenu();
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

