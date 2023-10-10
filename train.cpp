
#include "train.h"

// ȫ�ֱ�������
namespace
{
    // RewardMatrix: R����С 6*6
    // ValueMatrix : V����С 6*6
    Eigen::Matrix<map_type, 6, 6> RewardMatrix, ValueMatrix;

    // ѧϰ�ʡ�̽���ʡ�˥����
    double learningRate = 1, exploreRate = 0.1, gamaRate = 0.8;
    // ѵ������
    unsigned epoch = 200;
    // ��Ϸ���б�ʶ
    status stat = GAME_CTNE;

    // ȫ�����������
    std::default_random_engine randomEngine;
    std::uniform_real_distribution<double> randomExample(0, 1);
    std::uniform_int_distribution<unsigned> randomRoom(0, 4);
}

int main()
{
    /* ��յĵ�·ʹ�ø�������������Ϊ������� */
    // ��ʼ����������
    RewardMatrix << -1, -1, -1, 0, -1, -1,
        -1, -1, 0, 0, 0, -1,
        -1, 0, -1, -1, -1, 100,
        0, 0, -1, -1, -1, -1,
        -1, 0, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1;

    // ��ʼ����ֵ��
    ValueMatrix << 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0;

    /* ����ֵ�������������� */
    // ��ʦǿ�Ƽ�ֵ��
    // ValueMatrix << 0, 0, 0, 10, 0, 0,
    //     0, 0, 20, 0, 0, 0,
    //     0, 0, 0, 0, 0, 100,
    //     0, 10, 0, 0, 0, 0,
    //     0, 10, 0, 0, 0, 0,
    //     0, 0, 0, 0, 0, 0;

    Environment env;

    while (epoch--)
    {
        env.setEnv(randomRoom(randomEngine));
        std::cout << "Epoch: " << 200 - epoch << std::endl;

        env.setAction(std::make_pair(env.getContentment(), env.getContentment()));
        env.setValue(0);

        while (true)
        {
            col_type vec;

            if (randomExample(randomEngine) < exploreRate)
            {
                // ̽��ģʽ
                vec = GetRewardVector(env, -1);
#if PRINT
                std::cout << "̽��ģʽ" << std::endl;
#endif
            }
            else
            {
                // ����ģʽ
                vec = GetValueVector(env, -1);
#if PRINT
                std::cout << "����ģʽ" << std::endl;
#endif
            }

            act_type act = JudgeAction(env, vec);

            status st = TakeAction(env, act);

            // У�����
            if (stat != st)
                std::cout << "Proceecing Error: Stat != st" << std::endl;

            if (stat == GAME_OVER)
            {
                break;
            }
        }

        std::cout << "--------------------------------" << std::endl;
        PrintValueMatrix();
        std::cout << "--------------------------------" << std::endl;

        // ������Ϸ״̬
        stat = GAME_CTNE;
    }

    system("pause");
    return 0;
}

map_type ValueCalculate(Environment &env, act_type act, status &st)
{
    // �����Ϸ�Ѿ�������ֱ�ӷ��� 0.0
    if (st == GAME_OVER)
    {
        return 0.0;
    }

    env_type roomIndex = env.getContentment();
    map_type value = env.getValue();
    act_type recentAction = act;

    // У����Ϊ���͵�ǰ�����Ƿ�ƥ��
    assert(roomIndex == recentAction.first && "Please input correct input");

    // max( V(S',A')
    auto col_next = GetValueVector(env, act.second);

    // ���ﲻ����ֱ�� decltype(col_next(0)) ��õ�����
    map_type max_predict = col_next[0];

    for (std::size_t i = 0; i < col_next.size(); ++i)
    {
        if (col_next[i] > max_predict)
        {
            max_predict = col_next[i];
        }
    }

    // ��Ϸ���������
    if (act.second == 5)
    {
        max_predict = 100;
    }

    // Q_Learning: V(S,A) �� V(S,A) + ��( ��( max( V(S',A') ) ) - V(S,A) )
    map_type newValue = value + learningRate * (RewardMatrix(act.first, act.second) + gamaRate * max_predict - value);

    // ���� Value ���
    ValueMatrix(recentAction.first, recentAction.second) = newValue;

#if PRINT
    std::cout << "��ֵ: V(" << recentAction.first << "," << recentAction.second << ") Ϊ" << newValue << std::endl;
    // std::cout << "��ȡ Value:" << newValue << std::endl;
#endif

    return newValue;
}

status TakeAction(Environment &env, act_type act)
{
    // ��鵱ǰ��������Ϊ����Ƿ�ƥ��
    assert(act.first == env.getContentment() && "Please input correct room index.");

    // ��ֹ����ֵ����
    unsigned t_right = act.second;

    auto v = ValueCalculate(env, act, stat);

    env.setValue(v);
    env.setAction(act);

    // ���µ�ǰ����
    env.setEnv(t_right);

    if (t_right == 5)
    {
        // ��Ϸ����
        stat = GAME_OVER;

        // std::cout << "��Ϸ����" << std::endl;

        return GAME_OVER;
    }

    return GAME_CTNE;
}

col_type GetRewardVector(Environment &env, int iline)
{
    // ��ȡ��������һ��
    env_type roomIndex = env.getContentment();

    auto col = RewardMatrix.col(roomIndex);
    auto line = env.getContentment();

    if (iline != -1)
    {
        line = iline;
    }

    std::vector<map_type> vec;

    for (int i = 0; i < col.size(); ++i)
    {
        vec.push_back(RewardMatrix(line, i));
    }

    return vec;
}

col_type GetValueVector(Environment &env, int iline)
{
    // ��ȡ��������һ��
    env_type roomIndex = env.getContentment();

    auto col = RewardMatrix.col(roomIndex);
    auto line = env.getContentment();

    if (iline != -1)
    {
        line = iline;
    }

    std::vector<map_type> vec;

    for (int i = 0; i < col.size(); ++i)
    {
        vec.push_back(ValueMatrix(line, i));
    }

    return vec;
}

act_type JudgeAction(Environment &env, col_type &jgt)
{
    std::set<map_type> rst;
    std::vector<map_type> index;
    map_type max = 0;

    rst.clear();

    for (std::size_t i = 0; i < jgt.size(); ++i)
    {
        // �����������е� Value/Reward ֵ
        auto in = jgt[i];
        rst.insert(in);
    }

    // ȡ��������
    for (auto s : rst)
    {
        max = s;
    }

    // �ų�Ϊ���������
    for (std::size_t i = 0; i < jgt.size(); ++i)
    {
        if (jgt[i] == max && RewardMatrix(env.getContentment(), i) >= 0)
        {
            // ������ index �ŵ� vector ��
            index.push_back(i);
        }
    }

    act_type action;

    // ����ֵ��ֵ
    action.first = env.getContentment();

    // δ�ҵ�����
    if (index.size() == 0)
    {
        std::cout << "���ߴ��� - 0" << std::endl;
    }

    // ���ֻ��һ�����ֵ����ô����Ҫ���ѡ��
    if (index.size() == 1)
    {
        // ����ֵ��ֵ
        action.second = index.at(0);
    }
    else
    {
        // �ж��ֱ�������һ��ѡ
        auto count = index.size();

        std::uniform_int_distribution<unsigned> randomExample(0, count - 1);

        auto ran = randomExample(randomEngine);
        // ����ֵ��ֵ
        action.second = index.at(ran);
    }

    return action;
}

void PrintValueMatrix(void)
{
    std::cout << "Value matrix:" << std::endl;

    for (int i = 0; i < 6; ++i)
    {
        std::cout << "[ ";
        for (int j = 0; j < 6; ++j)
        {
            std::cout << ValueMatrix(i, j) << " ";
        }
        std::cout << "]\n";
    }
}