#include <eosiolib/currency.hpp>

#define GAME_SYMBOL S(4, EOS)

#define ITE1_CONTRACT N(iteblackhole)
#define ITE2_CONTRACT N(itedeathstar)
#define ITE3_CONTRACT N(iteecosystem)

using namespace eosio;
using namespace std;

namespace ite1
{
// air drop for ite1
struct userinfo
{
    uint64_t gameid;

    account_name owner;
    int64_t hodl;
    int64_t claim_status;

    uint64_t primary_key() const { return gameid; }

    EOSLIB_SERIALIZE(userinfo, (gameid)(owner)(hodl)(claim_status))
};

typedef multi_index<N(userinfo), userinfo> ite1user_index;
}

namespace ite2
{
// air drop for ite2
struct userinfo
{
    uint64_t gameid;

    account_name owner;
    int64_t hodl;                              // 持有智子数量
    int64_t claim_status;                      // 见证奖领取状态
    int64_t action_count;                      // 累计操作次数
    int64_t last_action_time = current_time(); // 上一次操作时间
    asset fee_amount = asset(0, GAME_SYMBOL);  // 累计手续费
    asset in = asset(0, GAME_SYMBOL);          // 累计收入
    asset out = asset(0, GAME_SYMBOL);         // 累计支出

    uint64_t primary_key() const { return gameid; }

    EOSLIB_SERIALIZE(userinfo, (gameid)(owner)(hodl)(claim_status)(action_count)(last_action_time)(fee_amount)(in)(out))
};

typedef multi_index<N(userinfo), userinfo> ite2user_index;
}

namespace ite3
{
// air drop for ite3
struct userinfo
{
    int64_t id;
    account_name owner;
    account_name referrer;                    // 推荐人
    int64_t hodl;                             // 持有ITE数量
    int64_t total_share_ite;                  // 累计推荐奖励ITE
    int64_t ref_count;                        // 累计推荐人
    int64_t claim_count;                      // 参与领分红次数
    int64_t action_count;                     // 累计操作次数
    int64_t last_action_time = now();         // 上一次操作时间
    int64_t last_claim_time;                  // 上一次领分红时间
    int64_t next_claim_time;                  // 下一次可领分红时间
    int64_t staked_time;                      // 锁币时间
    asset fee_amount = asset(0, GAME_SYMBOL); // 累计手续费
    asset in = asset(0, GAME_SYMBOL);         // 累计收入
    asset out = asset(0, GAME_SYMBOL);        // 累计支出
    asset claim = asset(0, GAME_SYMBOL);      // 累计分红
    int64_t join_time = now();

    uint64_t primary_key() const { return id; }

    EOSLIB_SERIALIZE(userinfo, (id)(owner)(referrer)(hodl)(total_share_ite)(ref_count)(claim_count)(action_count)(last_action_time)(last_claim_time)(next_claim_time)(staked_time)(fee_amount)(in)(out)(claim)(join_time))
};

typedef multi_index<N(userinfo), userinfo> ite3user_index;
}
