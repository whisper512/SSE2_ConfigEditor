#pragma once
#include <QtWidgets/QMainWindow>
#include <QAction>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMap>
#include <QLabel>
#include <QTimer>
#include <QPixmap>
#include <QGraphicsOpacityEffect>

#include "DataStructure.h"
#include "ui_SSE2_ConfigEditorMainWnd.h"

class SSE2_ConfigEditorMainWnd : public QMainWindow
{
    Q_OBJECT

public:
    SSE2_ConfigEditorMainWnd(QWidget* parent = nullptr);
    ~SSE2_ConfigEditorMainWnd();

private:
    Ui::SSE2_ConfigEditorMainWndClass ui;


    QAction* m_pActionOpenGamePath;
    QAction* m_pActionWriteConfig;
    QAction* m_pActionSaveBackup;
    QAction* m_pActionTip;

    QString m_strGamePath;                          // 游戏根目录
    QMap<eFaction, FactionData> m_factionDataMap;   // 全部6个阵营的数据
    FactionData* m_pCurrentFactionData = nullptr;   // 当前操作的阵营

    int m_iCurrentShipIndex;        // 当前主力舰索引 (0~4)
    int m_iCurrentLevelIndex;       // 当前等级索引 (0~9)
    int m_iCurrentTitanLevelIndex;  // 当前泰坦等级索引 (0~9)

private:
    void InitApplication();
    void InitMainWnd();
    void IntiEditor();
    void InitMenu();
    void InitData();

    void refreshCapitalshipCombox();
    void refreshTitanCombox();
    void ReadConfig();
    void ConnectSlots();

    // 单位限制
    void ParseUnitLimitConfigFromJson(const QJsonDocument& jsonDoc);
    void UpdateUnitsLimitData();
    void WriteUnitLimitConfigToJson();

    // 最大供应
    void ParseMaxSupplyConfigFromJson(const QJsonDocument& jsonDoc);
    void UpdateMaxSupplyData();
    void WriteMaxSupplyConfigToJson();

    // 起始资产
    void ParseDefaultStartingAssetsFromJson(const QJsonDocument& jsonDoc);
    void UpdateDefaultStartingAssetsData();
    void WriteDefaultStartingAssetsToJson();

    // 主力舰
    void ParseCapitalshipConfigFromJson(const QJsonDocument& jsonDocA, const QJsonDocument& jsonDocB,
        const QJsonDocument& jsonDocC, const QJsonDocument& jsonDocD, const QJsonDocument& jsonDocE);
    void UpdateCapitalshipData();
    void WriteCapitalshipConfigToJson();

    // 主力舰经验值
    void ParseCapitalshipExperienceFromJson(const QJsonDocument& jsonDocA, const QJsonDocument& jsonDocB,
        const QJsonDocument& jsonDocC, const QJsonDocument& jsonDocD, const QJsonDocument& jsonDocE);
    void WriteCapitalshipExperienceToJson();

    // 泰坦
    void ParseTitanConfigFromJson(const QJsonDocument& jsonDoc);
    void WriteTitanConfigToJson();
    void ParseTitanExperienceFromJson(const QJsonDocument& jsonDoc);
    void WriteTitanExperienceToJson();

    void OnOpenGamePath();
    void OnEditConfig();
    void OnSaveBackup();
    void OnTip();

protected:

private slots:
    void OnEditFinished();
    void OnCapitalshipTypeOrLevelChanged();
    void OnFactionChanged(int index);
    void OnTitanLevelChanged();
};
