#pragma once
#ifndef MODELS_H
#define MODELS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 1. 用户/患者档案链表节点 (对应用户端：建档)
typedef struct Patient {
    char id[20];            // 唯一患者ID
    char name[50];          // 姓名
    char gender[10];        // 性别
    int age;                // 年龄
    char allergy[100];      // 过敏史
    int isEmergency;        // 是否急诊 (1是 0否)
    double balance;         // 账户余额
    struct Patient* next;
} Patient, * PatientList;

// 2. 医护人员链表节点 (对应医护端/管理端：排班与人事)
typedef struct Staff {
    char id[20];            // 工号
    char password[50];      // 密码
    char name[50];          // 姓名
    char department[50];    // 所属科室
    char level[20];         // 医生级别 (如主任医师, 主治医师)
    struct Staff* next;
} Staff, * StaffList;

// 3. 药品库存链表节点 (对应管理端：药房管理)
typedef struct Medicine {
    char id[20];            // 药品编号
    char name[50];          // 药品名称
    int stock;              // 库存数量
    double price;           // 单价
    char expiryDate[20];    // 有效期
    struct Medicine* next;
} Medicine, * MedicineList;

// 4. 诊疗/挂号记录链表节点 (对应4类记录)
typedef struct Record {
    char recordId[30];      // 记录流水号
    int type;               // 1:挂号 2:看诊 3:检查 4:住院
    char patientId[20];     // 关联患者ID
    char staffId[20];       // 关联医生ID
    double cost;            // 费用
    int isPaid;             // 是否已缴费 (1是 0否)
    char description[200];  // 诊断描述/检查项目
    struct Record* next;
} Record, * RecordList;

// 5. 病床链表节点 (对应医护端：住院业务)
typedef struct Bed {
    int bedNumber;          // 床号
    int isOccupied;         // 状态：1占用 0空闲
    char patientId[20];     // 当前占用患者ID
    struct Bed* next;
} Bed, * BedList;

// 全局头指针声明 (在main.c中初始化)
extern PatientList patientHead;
extern StaffList staffHead;
extern MedicineList medicineHead;
extern RecordList recordHead;
extern BedList bedHead;

#endif
