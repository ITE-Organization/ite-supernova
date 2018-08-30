/**
*                         ______ _______ ______ 
*                        |_   _|__   __|  ____|
*                          | |    | |  | |__   
*                          | |    | |  |  __|  
*                         _| |_   | |  | |____ 
*                        |_____|  |_|  |______|
*                    
*
*        _______ _____ ____   _____ _____     _______ ___  _    _ _      
*      |__   __|__  _/ __ \ / ____|_   _|   |__   __|__ \ | |  | | |     
*          | |    | || |  | | |      | |        | |     ) | |  | | |     
*          | |    | || |  | | |      | |        | |    / /| |  | | |     
*          | |   _| || |__| | |____ _| |_       | |   / /_| |__| | |____ 
*          |_|  |_____\____/ \_____|_____|      |_|  |____|\____/|______|
*                                                                  
*
*          
*         Mirror, mirror on the wall, who's the hottest of them all ?
*
*
*    _                         _   _                      _                  _   
*  (_)                       | | | |                    | |                | |  
*    _ _ __    _ __ ___   __ _| |_| |__   __      _____  | |_ _ __ _   _ ___| |_ 
*  | | '_ \  | '_ ` _ \ / _` | __| '_ \  \ \ /\ / / _ \ | __| '__| | | / __| __|
*  | | | | | | | | | | | (_| | |_| | | |  \ V  V /  __/ | |_| |  | |_| \__ \ |_ 
*  |_|_| |_| |_| |_| |_|\__,_|\__|_| |_|   \_/\_/ \___|  \__|_|   \__,_|___/\__|
*
*/
#include <eosiolib/crypto.h>
#include <eosiolib/currency.hpp>
#include <eosiolib/transaction.hpp>
#include <itedata.cpp>
#include <math.h>

#define SOPHON S(4, SOPHON)
#define SATOSHI S(4, SATOSHI)
#define ITE_COIN S(4, ITECOIN)
#define GAME_SYMBOL S(4, EOS)

#define ITE1_CONTRACT N(iteblackhole)
#define ITE2_CONTRACT N(itedeathstar)
#define ITE3_CONTRACT N(iteecosystem)

#define WORM_HOLE N(itewormholes)

#define COIN_CONTRACT N(itecointoken)
#define TOKEN_CONTRACT N(eosio.token)

#define FEE_ACCOUNT N(itedecompany)
#define DEV_ACCOUNT N(itedeveloper)

typedef double real_type;

using namespace eosio;
using namespace std;

class ite4 : public contract
{
public:
  // parameters
  const uint64_t init_base_balance = 50 * 10000;          // 初始化筹码
  const uint64_t init_quote_balance = 1 * 5000 * 10000ll; // 初始保证金
  const uint64_t burn_price_ratio = 90;                   // 销毁价格比例
  const uint64_t end_prize_times = 10;                    // 终极大奖收益倍数
  const uint64_t end_prize_ratio = 10;                    // 终极大奖瓜分奖池的比例
  const uint64_t good_ending_ratio = 20;                  // 销毁智子总数 占 总智子数的比例。达到这个比例 游戏结束
  const uint64_t bad_ending_ratio = 85;                   // 已激活智子总数 占 当前有效智子数 (不含已销毁) 的比例。达到这个比例 游戏结束
  const uint64_t air_drop_step = 100;                     // 空投奖励的间隔
  const uint64_t action_limit_time = 15;                  // 操作冷却时间(s)
  const uint64_t max_operate_amount_ratio = 2;            // 每次可操作的智子数量, 不大于当前有效智子数 (不含已销毁) 的比例。(防止大户无脑绝杀，游戏变成猝死局)
  const uint64_t max_end_prize = 5000 * 10000;            // 终极大奖 设定为最大 5000 EOS
  const uint64_t burn_rate = 10;                          // 每次充能、还原的销毁比例
  const uint64_t game_start_time = 1535630400;            // 游戏开始时间 2018-08-30 20:00:00

  ite4(account_name self)
      : contract(self),
        _global(_self, _self),
        _games(_self, _self),
        _market(_self, _self),
        _users(_self, _self)
  {
    // Create a new global if not exists
    auto gl_itr = _global.begin();
    if (gl_itr == _global.end())
    {
      gl_itr = _global.emplace(_self, [&](auto &gl) {
        gl.gameid = 0;
        gl.air_drop_step = air_drop_step;
        gl.burn_price_ratio = burn_price_ratio;
        gl.end_prize_times = end_prize_times;
        gl.end_prize_ratio = end_prize_ratio;
        gl.good_ending_ratio = good_ending_ratio;
        gl.bad_ending_ratio = bad_ending_ratio;
        gl.action_limit_time = action_limit_time;
        gl.max_operate_amount_ratio = max_operate_amount_ratio;
        gl.max_end_prize = max_end_prize;
        gl.game_start_time = game_start_time;
      });
    }

    // Create a new game if not exists
    auto game_itr = _games.find(gl_itr->gameid);
    if (game_itr == _games.end())
    {
      game_itr = _games.emplace(_self, [&](auto &new_game) {
        new_game.gameid = gl_itr->gameid;
        auto cur_init_base_balance = init_base_balance + (init_base_balance / 10) * gl_itr->gameid;
        new_game.init_max = cur_init_base_balance;
        new_game.total_alive = cur_init_base_balance;
        auto cur_init_quote_balance = init_quote_balance + (init_quote_balance / 10) * gl_itr->gameid;
        new_game.quote_balance.amount = cur_init_quote_balance;
        new_game.init_quote_balance.amount = cur_init_quote_balance;
      });
    }

    // Create a new market if not exists
    auto market_itr = _market.find(gl_itr->gameid);
    if (market_itr == _market.end())
    {
      market_itr = _market.emplace(_self, [&](auto &m) {
        m.gameid = gl_itr->gameid;
        m.supply.amount = 100000000000000ll;
        m.supply.symbol = SATOSHI;
        auto cur_init_base_balance = init_base_balance + (init_base_balance / 10) * gl_itr->gameid;
        m.base.balance.amount = cur_init_base_balance;
        m.base.balance.symbol = SOPHON;
        auto cur_init_quote_balance = init_quote_balance + (init_quote_balance / 10) * gl_itr->gameid;
        m.quote.balance.amount = cur_init_quote_balance;
        m.quote.balance.symbol = GAME_SYMBOL;
      });
    }

    // Create default referrer if not exists
    user_resources_table default_ref_userres(_self, DEV_ACCOUNT);
    auto default_ref_res_itr = default_ref_userres.find(gl_itr->gameid);

    if (default_ref_res_itr == default_ref_userres.end())
    {
      player_index _players(_self, gl_itr->gameid);
      uint64_t player_id = _players.available_primary_key();

      _players.emplace(_self, [&](auto &new_player) {
        new_player.id = player_id;
        new_player.player_account = DEV_ACCOUNT;
      });

      auto user_itr = _users.find(DEV_ACCOUNT);
      if (user_itr == _users.end())
      {
        _users.emplace(_self, [&](auto &new_user) {
          new_user.id = DEV_ACCOUNT;
        });
      }

      default_ref_res_itr = default_ref_userres.emplace(_self, [&](auto &res) {
        res.id = player_id;
        res.gameid = gl_itr->gameid;
        res.referrer = DEV_ACCOUNT;
        res.owner = DEV_ACCOUNT;
      });
    }
  }

  void transfer(account_name from, account_name to, asset quantity, string memo)
  {
    if (from == _self || to != _self)
    {
      return;
    }

    eosio_assert(now() > game_start_time + random_offset(from), "The game will start at 2018-08-30 20:00:00");

    eosio_assert(quantity.is_valid(), "Invalid token transfer");
    eosio_assert(quantity.amount > 0, "Quantity must be positive");

    // only accepts GAME_SYMBOL for buy
    if (quantity.symbol == GAME_SYMBOL)
    {
      buy(from, quantity, memo);
    }
  }

  int64_t random_offset(account_name from)
  {
    checksum256 result;
    auto mixedBlock = tapos_block_prefix() * tapos_block_num() + from;

    const char *mixedChar = reinterpret_cast<const char *>(&mixedBlock);
    sha256((char *)mixedChar, sizeof(mixedChar), &result);
    const char *p64 = reinterpret_cast<const char *>(&result);
    auto x = 5, y = 120, z = 10;
    return (abs((int64_t)p64[x]) % (y)) + z;
  }

  void buy(account_name account, asset quant, string memo)
  {
    require_auth(account);
    eosio_assert(quant.amount > 0, "must purchase a positive amount");

    auto gl_itr = _global.begin();

    auto market_itr = _market.find(gl_itr->gameid);

    auto fee = quant;
    fee.amount = (fee.amount + 99) / 100; /// 1% fee (round up)
    auto action_total_fee = fee;

    auto quant_after_fee = quant;
    quant_after_fee.amount -= fee.amount;

    if (fee.amount > 0)
    {
      auto dev_fee = fee;
      dev_fee.amount = fee.amount * 30 / 100;
      fee.amount -= dev_fee.amount;

      auto ite2_fee = fee;
      ite2_fee.amount = fee.amount * 10 / 100;
      fee.amount -= ite2_fee.amount;

      auto ite3_fee = fee;
      ite3_fee.amount = fee.amount * 20 / 100;
      fee.amount -= ite3_fee.amount;

      action(
          permission_level{_self, N(active)},
          TOKEN_CONTRACT, N(transfer),
          make_tuple(_self, FEE_ACCOUNT, fee, string("buy fee")))
          .send();

      if (dev_fee.amount > 0)
      {
        action(
            permission_level{_self, N(active)},
            TOKEN_CONTRACT, N(transfer),
            make_tuple(_self, DEV_ACCOUNT, dev_fee, string("dev fee")))
            .send();
      }

      // send fee to ITE2
      if (ite2_fee.amount > 0)
      {
        auto n = name{ITE2_CONTRACT};
        string ite2_memo = n.to_string();
        action(
            permission_level{_self, N(active)},
            TOKEN_CONTRACT, N(transfer),
            make_tuple(_self, WORM_HOLE, ite2_fee, ite2_memo))
            .send();
      }

      // send fee to ITE3
      if (ite3_fee.amount > 0)
      {
        auto n = name{ITE3_CONTRACT};
        string ite3_memo = n.to_string();
        action(
            permission_level{_self, N(active)},
            TOKEN_CONTRACT, N(transfer),
            make_tuple(_self, WORM_HOLE, ite3_fee, ite3_memo))
            .send();
      }
    }

    int64_t sophon_out;

    _market.modify(market_itr, _self, [&](auto &es) {
      sophon_out = es.convert(quant_after_fee, SOPHON).amount;
    });

    eosio_assert(sophon_out > 0, "must reserve a positive amount");

    auto game_itr = _games.find(gl_itr->gameid);

    // max operate amount limit
    auto max_operate_amount = game_itr->total_alive / 100 * max_operate_amount_ratio;
    eosio_assert(sophon_out < max_operate_amount, "must reserve less than max operate amount");

    // burn
    int64_t burn = sophon_out * burn_rate / 100;

    _games.modify(game_itr, _self, [&](auto &game) {
      // counter limit
      if (quant_after_fee.amount > 99000)
      {
        game.counter++;
      }
      game.total_burn += burn;
      game.total_alive -= burn;
      game.total_reserved += sophon_out;
      game.quote_balance += quant_after_fee;
    });

    // mine ite coin
    asset mined_coin = asset(quant.amount, ITE_COIN);
    action(
        permission_level{_self, N(active)},
        COIN_CONTRACT, N(transfer),
        make_tuple(_self, account, mined_coin, string("mined ITE coin with playing ITE4 supernova")))
        .send();

    // calculate share amount
    auto share_sophon_out = sophon_out / 100;
    auto sophon_out_after_share = sophon_out;
    sophon_out_after_share -= share_sophon_out;

    user_resources_table userres(_self, account);
    auto res_itr = userres.find(gl_itr->gameid);
    if (res_itr == userres.end())
    {
      //get referrer
      account_name referrer = string_to_name(memo.c_str());
      user_resources_table ref_userres(_self, referrer);

      auto ref_res_itr = ref_userres.find(gl_itr->gameid);
      if (ref_res_itr == ref_userres.end())
      {
        referrer = DEV_ACCOUNT;
      }

      // add new player into players table
      player_index _players(_self, gl_itr->gameid);

      uint64_t player_id = _players.available_primary_key();

      _players.emplace(_self, [&](auto &new_player) {
        new_player.id = player_id;
        new_player.player_account = account;
      });

      res_itr = userres.emplace(_self, [&](auto &res) {
        res.id = player_id;
        res.gameid = gl_itr->gameid;
        res.referrer = referrer;
        res.owner = account;
        res.hodl = sophon_out_after_share;
        res.action_count++;
        res.fee_amount += action_total_fee;
        res.out += quant;
      });

      auto user_itr = _users.find(account);
      if (user_itr == _users.end())
      {
        _users.emplace(_self, [&](auto &new_user) {
          new_user.id = account;
        });
        ite1_air_drop(account);
        ite2_air_drop(account);
        ite3_air_drop(account);
      }

      if (referrer == DEV_ACCOUNT)
      {
        user_resources_table dev_ref_userres(_self, referrer);
        auto dev_ref_res_itr = dev_ref_userres.find(gl_itr->gameid);

        dev_ref_userres.modify(dev_ref_res_itr, _self, [&](auto &res) {
          res.hodl += share_sophon_out;
          res.total_share_sophon += share_sophon_out;
          res.ref_count++;
        });
      }
      else
      {
        ref_userres.modify(ref_res_itr, _self, [&](auto &res) {
          res.hodl += share_sophon_out;
          res.total_share_sophon += share_sophon_out;
          res.ref_count++;
        });
      }
    }
    else
    {
      // time limit
      auto time_diff = now() - res_itr->last_action_time;
      eosio_assert(time_diff > action_limit_time, "please wait a moment");

      userres.modify(res_itr, _self, [&](auto &res) {
        res.hodl += sophon_out_after_share;
        res.last_action_time = now();
        res.action_count++;
        res.fee_amount += action_total_fee;
        res.out += quant;
      });

      if (share_sophon_out > 0)
      {
        user_resources_table ref_userres(_self, res_itr->referrer);
        auto ref_res_itr = ref_userres.find(gl_itr->gameid);

        ref_userres.modify(ref_res_itr, _self, [&](auto &res) {
          res.hodl += share_sophon_out;
          res.total_share_sophon += share_sophon_out;
        });
      }
    }
    trigger_air_drop(account, quant_after_fee);
    trigger_game_over(account, quant_after_fee);
  }

  // air drop for ite1
  void ite1_air_drop(account_name account)
  {
    ite1::ite1user_index ite1user(ITE1_CONTRACT, account);
    auto user_itr = ite1user.begin();
    if (user_itr != ite1user.end())
    {
      if (user_itr->hodl > 0)
      {
        asset air_drop_itecoin_amount = asset(user_itr->hodl * 10000, ITE_COIN);
        action(
            permission_level{_self, N(active)},
            COIN_CONTRACT, N(transfer),
            make_tuple(_self, account, air_drop_itecoin_amount, string("ITE 1 air drop. Thank you for your support to the ITE team ")))
            .send();
      }
    }
  }

  // air drop for ite2
  void ite2_air_drop(account_name account)
  {
    ite2::ite2user_index ite2user(ITE2_CONTRACT, account);
    auto user_itr = ite2user.begin();
    if (user_itr != ite2user.end())
    {
      if (user_itr->hodl > 0)
      {
        asset air_drop_itecoin_amount = asset(user_itr->hodl * 10000, ITE_COIN);
        action(
            permission_level{_self, N(active)},
            COIN_CONTRACT, N(transfer),
            make_tuple(_self, account, air_drop_itecoin_amount, string("ITE 2 air drop. Thank you for your support to the ITE team ")))
            .send();
      }
    }
  }

  // air drop for ite3 ITE token holder
  void ite3_air_drop(account_name account)
  {
    ite3::ite3user_index ite3user(ITE3_CONTRACT, account);
    auto user_itr = ite3user.begin();
    if (user_itr != ite3user.end())
    {
      if (user_itr->hodl > 0)
      {
        asset air_drop_itecoin_amount = asset(user_itr->hodl * 10000 * 2, ITE_COIN);
        action(
            permission_level{_self, N(active)},
            COIN_CONTRACT, N(transfer),
            make_tuple(_self, account, air_drop_itecoin_amount, string("ITE 3 air drop. Thank you for your support to the ITE team ")))
            .send();
      }
    }
  }

  void sell(account_name account, int64_t amount)
  {
    require_auth(account);
    eosio_assert(amount > 0, "cannot sell negative amount");

    auto gl_itr = _global.begin();

    user_resources_table userres(_self, account);
    auto res_itr = userres.find(gl_itr->gameid);

    eosio_assert(res_itr != userres.end(), "no resource row");
    eosio_assert(res_itr->hodl >= amount, "insufficient quota");

    // time limit
    auto time_diff = now() - res_itr->last_action_time;
    eosio_assert(time_diff > action_limit_time, "please wait a moment");

    auto game_itr = _games.find(gl_itr->gameid);

    // max operate amount limit
    // auto max_operate_amount = game_itr->total_alive / 100 * max_operate_amount_ratio;
    // eosio_assert(amount < max_operate_amount, "must sell less than max operate amount");

    asset tokens_out;

    auto itr = _market.find(gl_itr->gameid);

    _market.modify(itr, _self, [&](auto &es) {
      tokens_out = es.convert(asset(amount, SOPHON), GAME_SYMBOL);
    });

    eosio_assert(tokens_out.amount > 0, "must payout a positive amount");

    // burn
    auto burn = amount * burn_rate / 100;

    auto max = game_itr->quote_balance - game_itr->init_quote_balance;

    if (tokens_out > max)
    {
      tokens_out = max;
    }

    _games.modify(game_itr, _self, [&](auto &game) {
      // counter limit
      if (tokens_out.amount > 99000)
      {
        game.counter++;
      }
      game.total_burn += burn;
      game.total_alive -= burn;
      game.total_reserved -= amount;
      game.quote_balance -= tokens_out;
    });

    auto fee = (tokens_out.amount + 99) / 100; /// 1% fee (round up)
    auto action_total_fee = fee;

    auto quant_after_fee = tokens_out;
    quant_after_fee.amount -= fee;

    userres.modify(res_itr, _self, [&](auto &res) {
      res.hodl -= amount;
      res.last_action_time = now();
      res.action_count++;
      res.fee_amount += asset(action_total_fee, GAME_SYMBOL);
      res.in += quant_after_fee;
    });

    action(
        permission_level{_self, N(active)},
        TOKEN_CONTRACT, N(transfer),
        make_tuple(_self, account, quant_after_fee, string("sell payout")))
        .send();

    if (fee > 0)
    {
      auto dev_fee = fee;
      dev_fee = fee * 30 / 100;
      fee -= dev_fee;

      auto ite2_fee = fee;
      ite2_fee = fee * 10 / 100;
      fee -= ite2_fee;

      auto ite3_fee = fee;
      ite3_fee = fee * 20 / 100;
      fee -= ite3_fee;

      action(
          permission_level{_self, N(active)},
          TOKEN_CONTRACT, N(transfer),
          make_tuple(_self, FEE_ACCOUNT, asset(fee, GAME_SYMBOL), string("sell fee")))
          .send();

      if (dev_fee > 0)
      {
        action(
            permission_level{_self, N(active)},
            TOKEN_CONTRACT, N(transfer),
            make_tuple(_self, DEV_ACCOUNT, asset(fee, GAME_SYMBOL), string("dev fee")))
            .send();
      }

      // send to ITE2
      if (ite2_fee > 0)
      {
        auto n = name{ITE2_CONTRACT};
        string ite2_memo = n.to_string();
        action(
            permission_level{_self, N(active)},
            TOKEN_CONTRACT, N(transfer),
            make_tuple(_self, WORM_HOLE, asset(ite2_fee, GAME_SYMBOL), ite2_memo))
            .send();
      }

      // send to ITE3
      if (ite3_fee > 0)
      {
        auto n = name{ITE3_CONTRACT};
        string ite3_memo = n.to_string();
        action(
            permission_level{_self, N(active)},
            TOKEN_CONTRACT, N(transfer),
            make_tuple(_self, WORM_HOLE, asset(ite3_fee, GAME_SYMBOL), ite3_memo))
            .send();
      }
    }
    trigger_air_drop(account, tokens_out);
    trigger_game_over(account, tokens_out);
  }

  void destroy(account_name account, int64_t amount)
  {
    require_auth(account);
    eosio_assert(amount > 0, "cannot destroy negative amount");

    auto gl_itr = _global.begin();

    user_resources_table userres(_self, account);
    auto res_itr = userres.find(gl_itr->gameid);

    eosio_assert(res_itr != userres.end(), "no resource row");
    eosio_assert(res_itr->hodl >= amount, "insufficient quota");

    // time limit
    auto time_diff = now() - res_itr->last_action_time;
    eosio_assert(time_diff > action_limit_time, "please wait a moment");

    auto game_itr = _games.find(gl_itr->gameid);

    // max operate amount limit
    auto max_operate_amount = game_itr->total_alive / 100 * max_operate_amount_ratio;
    eosio_assert(amount < max_operate_amount, "must destroy less than max operate amount");

    asset tokens_out;

    auto itr = _market.find(gl_itr->gameid);

    _market.modify(itr, 0, [&](auto &es) {
      tokens_out = es.convert(asset(amount, SOPHON), GAME_SYMBOL);
    });

    asset payout = tokens_out;
    payout.amount = tokens_out.amount / 100 * burn_price_ratio;

    eosio_assert(payout.amount > 0, "must payout a positive amount");

    auto max = game_itr->quote_balance - game_itr->init_quote_balance;

    if (payout > max)
    {
      payout = max;
    }

    userres.modify(res_itr, _self, [&](auto &res) {
      res.hodl -= amount;
      res.last_action_time = now();
      res.action_count++;
      res.in += payout;
    });

    asset destroy_balance = tokens_out - payout;

    // change game status
    _games.modify(game_itr, _self, [&](auto &game) {
      // counter limit
      if (destroy_balance.amount > 99000)
      {
        game.counter++;
      }
      game.total_burn += amount;
      game.total_alive -= amount;
      game.total_reserved -= amount;
      game.quote_balance -= payout;
      game.destroy_balance += destroy_balance;
    });

    // transfer payout to destroyer
    action(
        permission_level{_self, N(active)},
        TOKEN_CONTRACT, N(transfer),
        make_tuple(_self, account, payout, string("destroy payout")))
        .send();

    trigger_air_drop(account, payout);
    trigger_game_over(account, payout);
  }

  void trigger_air_drop(account_name account, asset quant)
  {
    auto gl_itr = _global.begin();
    const auto game_itr = _games.find(gl_itr->gameid);

    user_resources_table userres(_self, account);
    auto res_itr = userres.find(gl_itr->gameid);

    // check airdrop
    if (game_itr->counter > 0 && game_itr->counter % air_drop_step == 0)
    {
      // air drop rule: return total fee_amount of current user, the real reward <= fee_amount
      auto reward = res_itr->fee_amount * 5;

      auto max = game_itr->quote_balance - game_itr->init_quote_balance;

      if (reward > max)
      {
        reward = max;
      }

      // change market status
      auto market_itr = _market.find(gl_itr->gameid);

      // calculate the energy leakage
      int64_t sophon_out;

      _market.modify(market_itr, _self, [&](auto &es) {
        sophon_out = es.convert(reward, SOPHON).amount;
        if (sophon_out > 0)
        {
          reward = es.convert(asset(sophon_out, SOPHON), GAME_SYMBOL);
          reward = es.convert(asset(sophon_out, SOPHON), GAME_SYMBOL);
        }
      });

      userres.modify(res_itr, _self, [&](auto &res) {
        res.in += reward;
        res.fee_amount = asset(0, GAME_SYMBOL);
      });

      // change game status
      _games.modify(game_itr, _self, [&](auto &game) {
        game.total_burn += sophon_out;
        game.total_alive -= sophon_out;
        game.total_reserved -= sophon_out;
        game.quote_balance -= reward;
        game.total_lose += sophon_out;
      });

      bonus_index _bonus(_self, gl_itr->gameid);
      // create bonus record
      _bonus.emplace(_self, [&](auto &new_bonus) {
        new_bonus.count = game_itr->counter;
        new_bonus.gameid = gl_itr->gameid;
        new_bonus.owner = account;
        new_bonus.reward = reward;
        new_bonus.lose_amount = sophon_out;
      });

      if (reward.amount > 0)
      {
        action(
            permission_level{_self, N(active)},
            TOKEN_CONTRACT, N(transfer),
            make_tuple(_self, account, reward, string("air drop reward")))
            .send();
      }
    }
  }

  void trigger_game_over(account_name account, asset quant)
  {
    auto gl_itr = _global.begin();
    auto game_itr = _games.find(gl_itr->gameid);

    bool gameover = false;

    // 1. good ending of the game: total_burn >= {good_ending_ratio} % * init_max
    auto max_burn = game_itr->init_max / 100 * good_ending_ratio;

    if (game_itr->total_burn >= max_burn)
    {
      gameover = true;
    }

    // 2. bad ending of the game:  total_reserved >= total_alive * {bad_ending_ratio}%
    auto max_reserved = game_itr->total_alive / 100 * bad_ending_ratio;

    if (game_itr->total_reserved >= max_reserved)
    {
      gameover = true;
    }

    if (gameover)
    {
      // reward = {end_prize_times} * quant , but, <= {end_prize_ratio}% * quote_balance <= max_end_prize
      auto reward = quant;
      reward.amount = reward.amount * end_prize_times;

      auto max = game_itr->quote_balance - game_itr->init_quote_balance;
      max.amount = max.amount / 100 * end_prize_ratio;

      if (reward > max)
      {
        reward = max;
      }

      if (reward.amount > max_end_prize)
      {
        reward.amount = max_end_prize;
      }

      // calculate claim price : (quote_balance - init_quote_balance - reward)/ (total_reserved + total_lose)
      //（make sure the balance of account can buy back all sell）
      auto final_balance = game_itr->quote_balance - game_itr->init_quote_balance - reward;
      eosio_assert(final_balance.amount > 0, "shit happens");

      auto claim_price = final_balance;

      auto total_hold = game_itr->total_reserved + game_itr->total_lose;
      if (total_hold > 0)
      {
        claim_price.amount = claim_price.amount / total_hold;
      }

      eosio_assert(claim_price.amount > 0, "shit happens again");

      // transfer to hero
      action(
          permission_level{_self, N(active)},
          TOKEN_CONTRACT, N(transfer),
          make_tuple(_self, account, reward, string("hero reward")))
          .send();

      // change game status
      _games.modify(game_itr, _self, [&](auto &game) {
        game.status = 1;
        game.claim_price = claim_price;
        game.quote_balance -= reward;
        game.hero = account;
        game.hero_reward = reward;
        game.end_time = now();
      });

      // increment global game counter
      _global.modify(gl_itr, _self, [&](auto &gl) {
        gl.gameid++;
      });
    }
  }

  void claim(account_name account, int64_t gameid)
  {
    require_auth(account);

    auto gl_itr = _global.begin();
    auto game_itr = _games.find(gameid);

    eosio_assert(game_itr != _games.end(), "game 404 no found");
    eosio_assert(game_itr->status == 1, "no, pls claim after game over");

    user_resources_table userres(_self, account);
    auto res_itr = userres.find(gameid);

    eosio_assert(res_itr != userres.end(), "sorry, you are no a player of this game");
    eosio_assert(res_itr->claim_status == 0, "you had claimed this reward");

    auto reward = game_itr->claim_price;
    reward.amount = reward.amount * res_itr->hodl;

    userres.modify(res_itr, _self, [&](auto &res) {
      res.claim_status = 1;
    });

    if (reward.amount > 0)
    {
      action(
          permission_level{_self, N(active)},
          TOKEN_CONTRACT, N(transfer),
          make_tuple(_self, account, reward, string("claim reward")))
          .send();
    }
  }

private:
  // @abi table global i64
  struct global
  {
    uint64_t id = 0;
    uint64_t gameid;
    uint64_t air_drop_step;
    uint64_t burn_price_ratio;
    uint64_t end_prize_ratio;
    uint64_t end_prize_times;
    uint64_t good_ending_ratio;
    uint64_t bad_ending_ratio;
    uint64_t action_limit_time;
    uint64_t max_operate_amount_ratio;
    uint64_t max_end_prize;
    uint64_t game_start_time;

    uint64_t primary_key() const { return id; }

    EOSLIB_SERIALIZE(global, (id)(gameid)(air_drop_step)(burn_price_ratio)(end_prize_ratio)(end_prize_times)(good_ending_ratio)(bad_ending_ratio)(action_limit_time)(max_operate_amount_ratio)(max_end_prize)(game_start_time))
  };

  typedef eosio::multi_index<N(global), global> global_index;
  global_index _global;

  // @abi table game i64
  struct game
  {
    uint64_t gameid;
    uint64_t status;
    uint64_t counter;
    uint64_t init_max;
    uint64_t total_burn;
    uint64_t total_alive;
    uint64_t total_reserved;
    uint64_t total_lose;
    uint64_t start_time = now();
    uint64_t end_time;
    asset quote_balance = asset(0, GAME_SYMBOL);
    asset init_quote_balance = asset(0, GAME_SYMBOL);
    asset destroy_balance = asset(0, GAME_SYMBOL);
    asset claim_price = asset(0, GAME_SYMBOL);
    asset hero_reward = asset(0, GAME_SYMBOL);
    account_name hero;

    uint64_t primary_key() const { return gameid; }

    EOSLIB_SERIALIZE(game, (gameid)(status)(counter)(init_max)(total_burn)(total_alive)(total_reserved)(total_lose)(start_time)(end_time)(quote_balance)(init_quote_balance)(destroy_balance)(claim_price)(hero_reward)(hero))
  };

  typedef eosio::multi_index<N(game), game> game_index;
  game_index _games;

  // @abi table bonus i64
  struct bonus
  {
    uint64_t count;                       // 第几次
    uint64_t gameid;                      // 哪一局
    account_name owner;                   // 获奖者
    asset reward = asset(0, GAME_SYMBOL); // 获奖金额
    uint64_t lose_amount;                 // 游离智子数

    uint64_t primary_key() const { return count; }

    EOSLIB_SERIALIZE(bonus, (count)(gameid)(owner)(reward)(lose_amount))
  };

  typedef eosio::multi_index<N(bonus), bonus> bonus_index;

  // @abi table player i64
  struct player
  {
    int64_t id;
    account_name player_account;

    uint64_t primary_key() const { return id; }

    EOSLIB_SERIALIZE(player, (id)(player_account))
  };

  typedef eosio::multi_index<N(player), player> player_index;

  // @abi table users i64
  struct users
  {
    account_name id;
    uint64_t primary_key() const { return id; }
    EOSLIB_SERIALIZE(users, (id))
  };

  typedef eosio::multi_index<N(users), users> users_index;
  users_index _users;

  // @abi table userinfo i64
  struct userinfo
  {
    uint64_t gameid;

    int64_t id;
    account_name owner;
    account_name referrer;
    int64_t hodl;                             // 持有智子数量
    int64_t total_share_sophon;               // 累计推荐奖励智子
    int64_t ref_count;                        // 累计推荐人
    int64_t claim_status;                     // 见证奖领取状态
    int64_t action_count;                     // 累计操作次数
    int64_t last_action_time = now();         // 上一次操作时间
    asset fee_amount = asset(0, GAME_SYMBOL); // 累计手续费
    asset in = asset(0, GAME_SYMBOL);         // 累计收入
    asset out = asset(0, GAME_SYMBOL);        // 累计支出

    uint64_t primary_key() const { return gameid; }

    EOSLIB_SERIALIZE(userinfo, (gameid)(id)(owner)(referrer)(hodl)(total_share_sophon)(ref_count)(claim_status)(action_count)(last_action_time)(fee_amount)(in)(out))
  };

  typedef eosio::multi_index<N(userinfo), userinfo> user_resources_table;

  /**
    *  Uses Bancor math to create a 50/50 relay between two asset types. The state of the
    *  bancor exchange is entirely contained within this struct. There are no external
    *  side effects associated with using this API.
    */
  struct exchange_state
  {
    uint64_t gameid;

    asset supply;

    struct connector
    {
      asset balance;
      double weight = .5;

      EOSLIB_SERIALIZE(connector, (balance)(weight))
    };

    connector base;
    connector quote;

    uint64_t primary_key() const { return gameid; }

    asset convert_to_exchange(connector &c, asset in)
    {

      real_type R(supply.amount);
      real_type C(c.balance.amount + in.amount);
      real_type F(c.weight / 1000.0);
      real_type T(in.amount);
      real_type ONE(1.0);

      real_type E = -R * (ONE - pow(ONE + T / C, F));
      int64_t issued = int64_t(E);

      supply.amount += issued;
      c.balance.amount += in.amount;

      return asset(issued, supply.symbol);
    }

    asset convert_from_exchange(connector &c, asset in)
    {
      eosio_assert(in.symbol == supply.symbol, "unexpected asset symbol input");

      real_type R(supply.amount - in.amount);
      real_type C(c.balance.amount);
      real_type F(1000.0 / c.weight);
      real_type E(in.amount);
      real_type ONE(1.0);

      real_type T = C * (pow(ONE + E / R, F) - ONE);
      int64_t out = int64_t(T);

      supply.amount -= in.amount;
      c.balance.amount -= out;

      return asset(out, c.balance.symbol);
    }

    asset convert(asset from, symbol_type to)
    {
      auto sell_symbol = from.symbol;
      auto ex_symbol = supply.symbol;
      auto base_symbol = base.balance.symbol;
      auto quote_symbol = quote.balance.symbol;

      if (sell_symbol != ex_symbol)
      {
        if (sell_symbol == base_symbol)
        {
          from = convert_to_exchange(base, from);
        }
        else if (sell_symbol == quote_symbol)
        {
          from = convert_to_exchange(quote, from);
        }
        else
        {
          eosio_assert(false, "invalid sell");
        }
      }
      else
      {
        if (to == base_symbol)
        {
          from = convert_from_exchange(base, from);
        }
        else if (to == quote_symbol)
        {
          from = convert_from_exchange(quote, from);
        }
        else
        {
          eosio_assert(false, "invalid conversion");
        }
      }

      if (to != from.symbol)
        return convert(from, to);

      return from;
    }

    EOSLIB_SERIALIZE(exchange_state, (supply)(base)(quote))
  };

  typedef eosio::multi_index<N(market), exchange_state> market;
  market _market;
};

#define EOSIO_ABI_PRO(TYPE, MEMBERS)                                                                                                              \
  extern "C" {                                                                                                                                    \
  void apply(uint64_t receiver, uint64_t code, uint64_t action)                                                                                   \
  {                                                                                                                                               \
    auto self = receiver;                                                                                                                         \
    if (action == N(onerror))                                                                                                                     \
    {                                                                                                                                             \
      eosio_assert(code == N(eosio), "onerror action's are only valid from the \"eosio\" system account");                                        \
    }                                                                                                                                             \
    if ((code == TOKEN_CONTRACT && action == N(transfer)) || (code == self && (action == N(sell) || action == N(destroy) || action == N(claim)))) \
    {                                                                                                                                             \
      TYPE thiscontract(self);                                                                                                                    \
      switch (action)                                                                                                                             \
      {                                                                                                                                           \
        EOSIO_API(TYPE, MEMBERS)                                                                                                                  \
      }                                                                                                                                           \
    }                                                                                                                                             \
  }                                                                                                                                               \
  }

EOSIO_ABI_PRO(ite4, (transfer)(sell)(destroy)(claim))