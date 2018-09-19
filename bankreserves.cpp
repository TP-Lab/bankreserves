#include "bankreserves.hpp"

void bankreserves::withdraw() {
    require_auth2(_self, N(withdraw));
    
    asset ex_balance = get_balance(EX_ACCOUNT);
    asset bank_balance = get_balance(_self);

    asset sum = ex_balance + bank_balance;
    asset withdraw = (sum * 15 / 100) - ex_balance;
    eosio_assert(withdraw.amount > 0, "no need to withdraw");

    action(
            permission_level{_self, N(active)},
            N(eosio.token),
            N(transfer),
            make_tuple(_self, EX_ACCOUNT, withdraw, "withdraw")
    ).send();
}

void bankreserves::onblock() {
    require_auth(_self);

    transaction check;
    check.actions.emplace_back(permission_level{_self, N(active)}, _self, N(withdraw), std::make_tuple());
    check.delay_sec = 0;
    check.send(next_id(), _self);

    transaction out;
    out.actions.emplace_back(permission_level{_self, N(active)}, _self, N(onblock), std::make_tuple());
    out.delay_sec = 12 * 60 * 60;
    out.send(next_id(), _self);
}