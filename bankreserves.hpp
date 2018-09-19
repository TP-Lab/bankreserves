#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/singleton.hpp>
#include <eosiolib/transaction.hpp>

using namespace eosio;
using namespace std;

#define SN(X) (string_to_symbol(0, #X) >> 8)

const account_name EX_ACCOUNT = N(tokendapppub);

class bankreserves: public contract {
public:
    bankreserves(account_name self)
            : contract(self),
            sgt_config(_self, _self)
    {};
    void withdraw();
    void onblock();
private:
    struct account {
        asset    balance;
        uint64_t primary_key()const { return balance.symbol.name(); }
    };
    typedef multi_index<N(accounts), account> accounts;
    
    asset get_balance(account_name owner, account_name contract = N(eosio.token), symbol_name sym = SN(EOS)) const
    {
        accounts accountstable(contract, owner);
        auto ac = accountstable.find(sym);
        if (ac == accountstable.end()) {
            return asset();
        }
        return ac->balance;
    }

    struct st_config {
        uint64_t next_id = 0;
    };
    typedef singleton<N(config), st_config> tp_config;
    tp_config sgt_config;

    st_config get_config() {
        return sgt_config.get_or_create(_self, st_config{.next_id = 0});
    }

    uint64_t next_id() {
        st_config config = get_config();
        config.next_id++;
        sgt_config.set(config, _self);
        return config.next_id;
    }
};

EOSIO_ABI(bankreserves, (withdraw)(onblock))