#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "models.h"

// ==========================================
// 数据加载模块 (从TXT读取数据到内存链表)
// ==========================================

void loadPatients() {
    FILE* fp = fopen("patients.txt", "r");
    if (fp == NULL) {
        printf("【提示】未找到 patients.txt，将以空患者库启动。\n");
        return;
    }
    Patient* tail = patientHead;
    while (tail->next != NULL) tail = tail->next;

    Patient temp;
    // 【鲁棒性提升】严格校验 fscanf 返回值(7个字段)，防止文件格式损坏导致的死循环
    while (fscanf(fp, "%19s %99s %99s %d %999s %d %lf",
        temp.id, temp.name, temp.gender, &temp.age,
        temp.allergy, &temp.isEmergency, &temp.balance) == 7) {
        Patient* newNode = (Patient*)malloc(sizeof(Patient));
        *newNode = temp;
        newNode->next = NULL;
        tail->next = newNode;
        tail = newNode;
    }
    fclose(fp);
    printf("[-] 成功加载患者档案数据。\n");
}

void loadMedicines() {
    FILE* fp = fopen("medicines.txt", "r");
    if (fp == NULL) return;
    Medicine* tail = medicineHead;
    while (tail->next != NULL) tail = tail->next;

    Medicine temp;
    while (fscanf(fp, "%19s %99s %d %lf %99s",
        temp.id, temp.name, &temp.stock, &temp.price, temp.expiryDate) == 5) {
        Medicine* newNode = (Medicine*)malloc(sizeof(Medicine));
        *newNode = temp;
        newNode->next = NULL;
        tail->next = newNode;
        tail = newNode;
    }
    fclose(fp);
    printf("[-] 成功加载药房库存数据。\n");
}

void loadStaff() {
    FILE* fp = fopen("staff.txt", "r");
    if (fp == NULL) return;
    Staff* tail = staffHead;
    while (tail->next != NULL) tail = tail->next;

    Staff temp;
    while (fscanf(fp, "%19s %49s %99s %99s %99s",
        temp.id, temp.password, temp.name, temp.department, temp.level) == 5) {
        Staff* newNode = (Staff*)malloc(sizeof(Staff));
        *newNode = temp;
        newNode->next = NULL;
        tail->next = newNode;
        tail = newNode;
    }
    fclose(fp);
    printf("[-] 成功加载医护人员数据。\n");
}

void loadRecords() {
    FILE* fp = fopen("records.txt", "r");
    if (fp == NULL) return;
    Record* tail = recordHead;
    while (tail->next != NULL) tail = tail->next;

    Record temp;
    while (fscanf(fp, "%29s %d %19s %19s %lf %d %199s",
        temp.recordId, &temp.type, temp.patientId, temp.staffId,
        &temp.cost, &temp.isPaid, temp.description) == 7) {
        Record* newNode = (Record*)malloc(sizeof(Record));
        *newNode = temp;
        newNode->next = NULL;
        tail->next = newNode;
        tail = newNode;
    }
    fclose(fp);
    printf("[-] 成功加载诊疗流水记录。\n");
}

void loadAllDataFromTxt() {
    printf("\n--- 正在初始化系统数据 ---\n");
    loadPatients();
    loadMedicines();
    loadStaff();
    loadRecords();
    printf("--------------------------\n");
}

// ==========================================
// 数据保存模块 (将内存链表写入TXT)
// ==========================================

void savePatients() {
    FILE* fp = fopen("patients.txt", "w");
    if (fp == NULL) return;
    Patient* current = patientHead->next;
    while (current != NULL) {
        fprintf(fp, "%s %s %s %d %s %d %.2f\n",
            current->id, current->name, current->gender, current->age,
            current->allergy, current->isEmergency, current->balance);
        current = current->next;
    }
    fclose(fp);
}

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

void saveStaff() {
    FILE* fp = fopen("staff.txt", "w");
    if (fp == NULL) return;
    Staff* current = staffHead->next;
    while (current != NULL) {
        fprintf(fp, "%s %s %s %s %s\n",
            current->id, current->password, current->name, current->department, current->level);
        current = current->next;
    }
    fclose(fp);
}

void saveRecords() {
    FILE* fp = fopen("records.txt", "w");
    if (fp == NULL) return;
    Record* current = recordHead->next;
    while (current != NULL) {
        // 【鲁棒性注意】文本中不能含有空格，否则下次 fscanf 读取会断掉
        fprintf(fp, "%s %d %s %s %.2f %d %s\n",
            current->recordId, current->type, current->patientId, current->staffId,
            current->cost, current->isPaid, current->description);
        current = current->next;
    }
    fclose(fp);
}

void saveAllDataToTxt() {
    savePatients();
    saveMedicines();
    saveStaff();
    saveRecords();
    printf("【系统】所有数据已成功保存至本地 TXT 文件。\n");
}