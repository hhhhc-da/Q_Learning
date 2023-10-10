#ifndef TRAIN_NANOKA_H
#define TRAIN_NANOKA_H

// ������ͷ�ļ�
#include "Eigen/Eigen/Eigen"
#include <iostream>
#include <string>
#include <utility>
#include <set>
#include <vector>
#include <random>
#include <assert.h>
#include <vector>

// ������������

// ��Ϊ����
using act_type = std::pair<unsigned, unsigned>;
// ��������
using env_type = unsigned;
// ��ֵ����
using map_type = double;
// ��Ϸ״̬����
using status = unsigned;
// ������
using col_type = std::vector<map_type>;

#define GAME_OVER 0
#define GAME_CTNE 1

// ��Ϊ���ѱ�����

// ������
class Environment
{
public:
    // Ĭ�ϳ�ʼ������
    Environment() : roomIndex(0), reccentAction(std::make_pair(0, 0)), value(0)
    {
    }

    Environment(Environment &) = default;
    Environment(Environment &&) = default;
    ~Environment() = default;

    // ��ȡ����
    inline env_type getContentment(void)
    {
        return roomIndex;
    }

    inline act_type getRecentAction(void)
    {
        return reccentAction;
    }

    inline map_type getValue(void)
    {
        return value;
    }

    inline void setEnv(env_type ID)
    {
        roomIndex = ID;
    }

    inline void setValue(map_type v)
    {
        value = v;
    }

    inline void setAction(act_type a)
    {
        reccentAction.first = a.first;
        reccentAction.second = a.second;
    }

private:
    // ��ǰ�����
    env_type roomIndex;
    // �ϴ����Ķ���
    act_type reccentAction;
    // ��ǰӵ�е� Value ֵ
    map_type value;
};

// �ⲿ��������

// ���㵱ǰ����һ�������ļ�ֵ
map_type
ValueCalculate(Environment &env, act_type act, status &st);
// ��ʼִ���ж����Ҹ��� Value ��
status TakeAction(Environment &env, act_type act);
// ��ȡ Reward ��������
col_type GetRewardVector(Environment &env, int iline);
// ��ȡ Value ��������
col_type GetValueVector(Environment &env, int iline);
// ������������������Ϊ���ߣ����������Ϊ
act_type JudgeAction(Environment &env, col_type &jgt);
// ��ӡ Value ��
void PrintValueMatrix(void);

#endif