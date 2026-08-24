#pragma once

#include "EncryptionManager.h"

namespace NepBill
{
    const std::string JWT_COOKIE_KEY = "69f2e46b088b2b73d2a71d87cfb8813a34a819b16892e6205cfbf03738e4df45";
    constexpr int MaxLoginCookieAge = 60 * 60 * 24 * 30; // 30 days

    bool IsAccountIdEmail(std::string Id);

    crow::response IndexDeafult(NepBill::App &Backend, const crow::request &Req);
    crow::response LoggedIn(NepBill::App &Backend, const crow::request &Req);
    crow::response ApiLogin(NepBill::App &Backend, const crow::request &Req);
    crow::response AccountQueryInfo(NepBill::App &Backend, const crow::request &Req);
    crow::response AccountLogout(const crow::request &Req);
    crow::response SubmitContactForm(NepBill::App &Backend, const crow::request &Req);
    crow::response AdminContactFormsQuery(NepBill::App &Backend, const crow::request &Req);
    crow::response AdminContactFormQueryDetail(NepBill::App &Backend, const crow::request &Req);
    crow::response AdminContactFormEditSave(NepBill::App &Backend, const crow::request &Req);
    crow::response AdminContactFormRegisterQuery(NepBill::App &Backend, const crow::request &Req);
    crow::response AdminContactFormRegisterApprove(NepBill::App &Backend, const crow::request &Req);


} // namespace NepBill
