#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "models.h"

// 内部工具函数：生成唯一的记录流水号
void generateRecordID(char* buffer) {
    static int recCount = 1000;
    sprintf(buffer, "R2025%04d", recCount++);
}

// 1. 开具处方与库存校验
void prescribeMedicine(const char* currentDoctorId, const char* currentPatientId) {
    char searchKey[50];

    printf("\n==================================\n");
    printf("      处方与发药 - 开具处方      \n");
    printf("==================================\n");
    printf("请输入要开具的药品名称或编号进行检索: ");
    scanf("%s", searchKey);

    Medicine* med = medicineHead->next;
    Medicine* targetMed = NULL;

    while (med != NULL) {
        if (strcmp(med->name, searchKey) == 0 || strcmp(med->id, searchKey) == 0) {
            targetMed = med;
            break;
        }
        med = med->next;
    }

    if (targetMed == NULL) {
        printf("【系统提示】未找到名称或编号为 '%s' 的药品，请核对后重试！\n", searchKey);
        return;
    }

    printf("\n【查询成功】\n药品名称: %s\n当前库存: %d\n单价: %.2f元\n",
        targetMed->name, targetMed->stock, targetMed->price);

    int quantity;
    printf("请输入开具数量: ");
    if (scanf("%d", &quantity) != 1 || quantity <= 0) {
        while (getchar() != '\n');
        printf("【操作失误拦截】输入数量无效，请输入大于0的整数！\n");
        return;
    }

    if (quantity > targetMed->stock) {
        printf("\n【拦截告警】库存不足！该药品仅剩 %d 份。\n", targetMed->stock);
        return;
    }

    Record* newRecord = (Record*)malloc(sizeof(Record));
    generateRecordID(newRecord->recordId);
    newRecord->type = 2;
    strcpy(newRecord->patientId, currentPatientId);
    strcpy(newRecord->staffId, currentDoctorId);
    newRecord->cost = quantity * targetMed->price;
    newRecord->isPaid = 0;

    sprintf(newRecord->description, "处方药: %s x %d", targetMed->name, quantity);
    newRecord->next = NULL;

    Record* temp = recordHead;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = newRecord;

    printf("\n【开具成功】已生成待缴费账单: %s，总费用: %.2f 元\n", newRecord->recordId, newRecord->cost);
}

// 2. 医护端主菜单调度
void staffTerminal() {
    int choice;
    char doctorId[20] = "D001"; // 模拟当前登录的医生工号
    char patientId[20];

    while (1) {
        printf("\n--- 医护端 (医生工作台) ---\n");
        printf(" 1. 开具处方与发药\n");
        printf(" 0. 返回主菜单\n");
        printf("请选择: ");
        scanf("%d", &choice);

        switch (choice) {
        case 1:
            printf("请输入当前就诊患者的ID: ");
            scanf("%s", patientId);
            prescribeMedicine(doctorId, patientId);
            break;
        case 0:
            return;
        default:
            printf("输入无效！\n");
        }
    }
}