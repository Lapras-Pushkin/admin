#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "models.h"
// 如果有 user.h，这里也要 include 进来

// ==========================================
// 辅助函数
// ==========================================
// 生成唯一的患者ID
void generatePatientID(char* idBuffer) {
    static int counter = 1000;
    sprintf(idBuffer, "P2025%04d", counter++);
}

// 根据患者ID在链表中查找患者节点
Patient* findPatientById(const char* pid) {
    Patient* p = patientHead->next;
    while (p != NULL) {
        if (strcmp(p->id, pid) == 0) {
            return p;
        }
        p = p->next;
    }
    return NULL;
}

// ==========================================
// 核心业务功能
// ==========================================
// 1. 账户注册与建档
void registerPatient() {
    Patient* newPatient = (Patient*)malloc(sizeof(Patient));
    newPatient->next = NULL;

    printf("\n--- 账户注册与建档 ---\n");
    printf("请选择就诊类型 (1.普通 2.急诊): ");
    int type;
    scanf("%d", &type);
    newPatient->isEmergency = (type == 2) ? 1 : 0;

    printf("请输入姓名: ");
    scanf("%s", newPatient->name);
    printf("请输入性别: ");
    scanf("%s", newPatient->gender);

    if (!newPatient->isEmergency) {
        printf("请输入年龄: ");
        scanf("%d", &newPatient->age);
        printf("请输入过敏史 (无则填无): ");
        scanf("%s", newPatient->allergy);
    }
    else {
        newPatient->age = -1;
        strcpy(newPatient->allergy, "未知(急诊未录)");
    }

    generatePatientID(newPatient->id);
    newPatient->balance = 0.0;

    // 尾插法
    Patient* temp = patientHead;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = newPatient;

    printf("【成功】系统已为您生成唯一患者ID: %s\n", newPatient->id);
    printf("档案建立成功！\n");
}

// 2. 财务与费用中心
void financeCenter(const char* currentPatientId) {
    Patient* currentPatient = findPatientById(currentPatientId);
    if (currentPatient == NULL) {
        printf("【系统错误】未找到您的档案信息！\n");
        return;
    }

    printf("\n==================================\n");
    printf("        财务与费用中心        \n");
    printf("==================================\n");
    printf("当前账户余额: %.2f 元\n", currentPatient->balance);

    Record* rec = recordHead->next;
    int hasUnpaid = 0;

    printf("\n--- 您的待缴费账单 ---\n");
    while (rec != NULL) {
        if (strcmp(rec->patientId, currentPatientId) == 0 && rec->isPaid == 0) {
            printf("流水号: %s | 描述: %s | 金额: %.2f元\n",
                rec->recordId, rec->description, rec->cost);
            hasUnpaid = 1;
        }
        rec = rec->next;
    }

    if (!hasUnpaid) {
        printf("【提示】您当前没有未缴费的账单。祝您早日康复！\n");
        return;
    }

    char targetRecordId[30];
    printf("\n请输入要缴费的账单流水号 (输入0返回上一级): ");
    scanf("%s", targetRecordId);
    if (strcmp(targetRecordId, "0") == 0) return;

    Record* targetRecord = NULL;
    rec = recordHead->next;
    while (rec != NULL) {
        if (strcmp(rec->recordId, targetRecordId) == 0 &&
            rec->isPaid == 0 &&
            strcmp(rec->patientId, currentPatientId) == 0) {
            targetRecord = rec;
            break;
        }
        rec = rec->next;
    }

    if (targetRecord == NULL) {
        printf("【错误】未找到该流水号的待缴费账单，请核对输入！\n");
        return;
    }

    while (currentPatient->balance < targetRecord->cost) {
        printf("\n【余额不足】账单金额: %.2f元，您的当前余额: %.2f元\n",
            targetRecord->cost, currentPatient->balance);
        printf("请先充值！请输入充值金额 (输入0取消): ");

        double rechargeAmount;
        if (scanf("%lf", &rechargeAmount) != 1) {
            while (getchar() != '\n');
            printf("【操作失误拦截】输入无效，请输入正确的金额！\n");
            continue;
        }
        if (rechargeAmount == 0) return;
        if (rechargeAmount < 0) {
            printf("【拦截】充值金额必须大于0！\n");
            continue;
        }
        currentPatient->balance += rechargeAmount;
        printf("【充值成功】当前余额: %.2f 元\n", currentPatient->balance);
    }

    currentPatient->balance -= targetRecord->cost;
    targetRecord->isPaid = 1;
    printf("\n【缴费成功】已扣除 %.2f 元，当前余额: %.2f 元。\n", targetRecord->cost, currentPatient->balance);

    char medName[50];
    int qty = 0;
    if (sscanf(targetRecord->description, "处方药: %s x %d", medName, &qty) == 2) {
        Medicine* m = medicineHead->next;
        while (m != NULL) {
            if (strcmp(m->name, medName) == 0) {
                m->stock -= qty;
                printf("【系统底层通知】已扣减药房库存: %s %d份。请前往窗口取药。\n", m->name, qty);
                break;
            }
            m = m->next;
        }
    }
}

// ==========================================
// 模块三：个人医疗档案库 (历史记录分类查询)
// ==========================================

// 内部核心工具函数：根据类型过滤并打印记录 (O(N) 时间复杂度遍历)
// 参数 type: 1挂号, 2看诊/处方, 3检查, 4住院, 0表示查询所有(消费明细)
void printRecordsByType(const char* patientId, int type, const char* title) {
    printf("\n>>> %s <<<\n", title);
    printf("%-15s %-10s %-25s %-10s %-10s\n", "流水号", "经办医生", "详情描述", "费用(元)", "状态");
    printf("----------------------------------------------------------------------\n");

    Record* rec = recordHead->next; // 从头结点的下一个真实数据开始
    int count = 0;
    double typeTotalCost = 0.0;

    while (rec != NULL) {
        // 核心过滤条件：患者ID匹配 && (传入类型为0 或者 类型完全匹配)
        if (strcmp(rec->patientId, patientId) == 0 && (type == 0 || rec->type == type)) {
            printf("%-15s %-10s %-25s %-10.2f %-10s\n",
                rec->recordId,
                rec->staffId,
                rec->description,
                rec->cost,
                rec->isPaid ? "已缴费" : "待缴费"); // 三目运算符简化状态输出

            count++;
            typeTotalCost += rec->cost;
        }
        rec = rec->next;
    }

    if (count == 0) {
        printf("  (空) 暂无相关记录。\n");
    }
    printf("----------------------------------------------------------------------\n");
    printf("共找到 %d 条记录，该项累计产生费用: %.2f 元\n", count, typeTotalCost);
}

// ==========================================
// 模块四：自助预约与挂号 (生成第1类记录)
// ==========================================
void bookAppointment(const char* currentPatientId) {
    Patient* currentPatient = findPatientById(currentPatientId);
    if (currentPatient == NULL) {
        printf("【系统错误】未找到您的档案信息，请先注册建档！\n");
        return;
    }

    printf("\n==================================\n");
    printf("        自助预约与挂号中心        \n");
    printf("==================================\n");

    // 1. 浏览科室与当值医生
    Staff* doctor = staffHead->next;
    int doctorCount = 0;

    printf("%-10s %-15s %-15s %-15s\n", "医生工号", "姓名", "科室", "级别");
    printf("--------------------------------------------------\n");
    while (doctor != NULL) {
        printf("%-10s %-15s %-15s %-15s\n",
            doctor->id, doctor->name, doctor->department, doctor->level);
        doctorCount++;
        doctor = doctor->next;
    }
    printf("--------------------------------------------------\n");

    if (doctorCount == 0) {
        printf("【抱歉】当前无当值医生排班，请稍后再试。\n");
        return;
    }

    // 2. 选择医生挂号
    char targetDoctorId[20];
    printf("请输入您要预约的【医生工号】 (输入0退出): ");
    scanf("%s", targetDoctorId);
    if (strcmp(targetDoctorId, "0") == 0) return;

    // 校验医生是否存在
    doctor = staffHead->next;
    Staff* selectedDoctor = NULL;
    while (doctor != NULL) {
        if (strcmp(doctor->id, targetDoctorId) == 0) {
            selectedDoctor = doctor;
            break;
        }
        doctor = doctor->next;
    }

    if (selectedDoctor == NULL) {
        printf("【错误】未找到该工号的医生，挂号失败！\n");
        return;
    }

    // 3. 动态计算挂号费 (体现人性化设计：不同级别收费不同)
    double regFee = 15.0; // 默认普通号
    if (strstr(selectedDoctor->level, "主任") != NULL) {
        regFee = 50.0; // 专家号
    }
    else if (strstr(selectedDoctor->level, "副主任") != NULL) {
        regFee = 30.0;
    }

    // 4. 生成第1类记录：挂号记录待缴费账单
    Record* newRecord = (Record*)malloc(sizeof(Record));

    // 我们需要声明一下 generateRecordID，因为之前它写在了 staff.c 里
    // 为了简单，这里直接复刻一个简单的流水号生成器，或者你可以把它移到 utils.c 里共享
    static int regCount = 5000;
    sprintf(newRecord->recordId, "REG2025%04d", regCount++);

    newRecord->type = 1; // 1 代表挂号记录
    strcpy(newRecord->patientId, currentPatientId);
    strcpy(newRecord->staffId, selectedDoctor->id);
    newRecord->cost = regFee;
    newRecord->isPaid = 0; // 0 表示待缴费

    sprintf(newRecord->description, "挂号费: %s-%s", selectedDoctor->department, selectedDoctor->name);
    newRecord->next = NULL;

    // 尾插法加入全局记录链表
    Record* temp = recordHead;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = newRecord;

    // 5. 成功提示
    printf("\n【挂号成功】\n");
    printf("已成功预约 %s 的 %s 医生。\n", selectedDoctor->department, selectedDoctor->name);
    printf("本次挂号费用: %.2f 元\n", regFee);
    printf("温馨提示：请前往【财务与费用中心】完成缴费，缴费后方可前往诊室候诊！\n");
}


// 档案库主菜单
void medicalRecords(const char* currentPatientId) {
    // 校验患者身份
    Patient* currentPatient = findPatientById(currentPatientId);
    if (currentPatient == NULL) {
        printf("【系统错误】未找到您的档案信息，请先注册建档！\n");
        return;
    }

    int choice;
    while (1) {
        printf("\n==================================\n");
        printf("  个人医疗档案库 (患者ID: %s)  \n", currentPatientId);
        printf("==================================\n");
        printf(" 1. 挂号记录查询 (第1类记录)\n");
        printf(" 2. 看诊与处方记录 (第2类记录)\n");
        printf(" 3. 检查记录查询 (第3类记录)\n");
        printf(" 4. 住院记录查询 (第4类记录)\n");
        printf(" 5. 消费报表与所有明细查询\n");
        printf(" 0. 返回上一级\n");
        printf("请选择查询档案类型: ");

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            printf("【拦截】输入非法，请输入对应数字！\n");
            continue;
        }

        switch (choice) {
        case 1: printRecordsByType(currentPatientId, 1, "挂号记录历史"); break;
        case 2: printRecordsByType(currentPatientId, 2, "看诊与处方记录历史"); break;
        case 3: printRecordsByType(currentPatientId, 3, "检查单记录历史"); break;
        case 4: printRecordsByType(currentPatientId, 4, "住院与床位记录历史"); break;
        case 5: printRecordsByType(currentPatientId, 0, "个人消费报表与全部明细"); break;
        case 0: return;
        default: printf("【错误】无效的选项，请重新输入！\n");
        }
    }
}

// 3. 用户端主菜单调度
// ==========================================
// 用户端主菜单调度
// ==========================================
void userTerminal() {
    int choice;
    char currentId[20];

    while (1) {
        printf("\n--- 用户端 (患者自助终端) ---\n");
        printf(" 1. 账户注册与建档\n");
        printf(" 2. 自助预约与挂号\n");         // <--- 新增挂号入口
        printf(" 3. 财务与费用中心 (充值/缴费)\n");
        printf(" 4. 个人医疗档案库 (查询历史记录)\n");
        printf(" 0. 返回主菜单\n");
        printf("请选择操作: ");

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            printf("输入非法，请输入数字！\n");
            continue;
        }

        switch (choice) {
        case 1:
            registerPatient();
            break;
        case 2:
            printf("请输入您的患者ID以登录: ");
            scanf("%s", currentId);
            bookAppointment(currentId);
            break;
        case 3:
            printf("请输入您的患者ID以登录: ");
            scanf("%s", currentId);
            financeCenter(currentId);
            break;
        case 4:
            printf("请输入您的患者ID以验证身份: ");
            scanf("%s", currentId);
            medicalRecords(currentId);
            break;
        case 0:
            return;
        default:
            printf("输入无效！\n");
        }
    }
}