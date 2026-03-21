#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "models.h"

// ==========================================
// 数据加载模块 (从TXT读取数据到内存链表)
// ==========================================

// 1. 加载患者数据
void loadPatients() {
    FILE* fp = fopen("patients.txt", "r");
    if (fp == NULL) {
        printf("【提示】未找到 patients.txt，将以空患者库启动。\n");
        return;
    }

    Patient* tail = patientHead; // 使用尾指针优化插入效率
    // 定位到当前链表的实际尾部 (防止重复加载)
    while (tail->next != NULL) {
        tail = tail->next;
    }

    // 循环读取直到文件末尾 (EOF)
    Patient temp;
    // 假设 txt 格式为: ID 姓名 性别 年龄 过敏史 是否急诊 余额
    while (fscanf(fp, "%s %s %s %d %s %d %lf",
        temp.id, temp.name, temp.gender, &temp.age,
        temp.allergy, &temp.isEmergency, &temp.balance) != EOF) {

        // 为新节点分配内存
        Patient* newNode = (Patient*)malloc(sizeof(Patient));
        *newNode = temp; // 结构体整体赋值
        newNode->next = NULL;

        // 尾插法接入链表
        tail->next = newNode;
        tail = newNode;
    }
    fclose(fp);
    printf("【成功】患者档案数据加载完毕。\n");
}

// 2. 加载药品库数据
void loadMedicines() {
    FILE* fp = fopen("medicines.txt", "r");
    if (fp == NULL) {
        printf("【提示】未找到 medicines.txt，将以空药房启动。\n");
        return;
    }

    Medicine* tail = medicineHead;
    while (tail->next != NULL) tail = tail->next;

    Medicine temp;
    // 假设 txt 格式为: 编号 名称 库存 单价 有效期
    while (fscanf(fp, "%s %s %d %lf %s",
        temp.id, temp.name, &temp.stock,
        &temp.price, temp.expiryDate) != EOF) {

        Medicine* newNode = (Medicine*)malloc(sizeof(Medicine));
        *newNode = temp;
        newNode->next = NULL;

        tail->next = newNode;
        tail = newNode;
    }
    fclose(fp);
    printf("【成功】药房库存数据加载完毕。\n");
}

// 统一对外接口：系统启动时调用
void loadAllDataFromTxt() {
    printf("\n--- 正在初始化系统数据 ---\n");
    loadPatients();
    loadMedicines();
    // loadStaff(); // 按需补充
    // loadRecords(); // 按需补充
    printf("--------------------------\n");
}

// ==========================================
// 数据保存模块 (将内存链表写入TXT)
// ==========================================

// 1. 保存患者数据
void savePatients() {
    FILE* fp = fopen("patients.txt", "w"); // "w"模式会覆盖原文件，实现数据更新
    if (fp == NULL) {
        printf("【严重错误】无法写入 patients.txt！\n");
        return;
    }

    Patient* current = patientHead->next; // 跳过头结点
    while (current != NULL) {
        fprintf(fp, "%s %s %s %d %s %d %.2f\n",
            current->id, current->name, current->gender, current->age,
            current->allergy, current->isEmergency, current->balance);
        current = current->next;
    }
    fclose(fp);
}

// 2. 保存药品数据
void saveMedicines() {
    FILE* fp = fopen("medicines.txt", "w");
    if (fp == NULL) return;

    Medicine* current = medicineHead->next;
    while (current != NULL) {
        fprintf(fp, "%s %s %d %.2f %s\n",
            current->id, current->name, current->stock,
            current->price, current->expiryDate);
        current = current->next;
    }
    fclose(fp);
}

// 统一对外接口：系统安全退出时调用
void saveAllDataToTxt() {
    savePatients();
    saveMedicines();
    // saveStaff(); // 按需补充
    // saveRecords(); // 按需补充
    printf("【系统】所有数据已成功保存至本地 TXT 文件。\n");
}