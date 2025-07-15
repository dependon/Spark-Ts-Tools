#include "appobject.h"

AppObject *AppObject::m_app = nullptr;
AppObject *AppObject::instance()
{
    if(!m_app)
    {
        m_app = new AppObject();
    }
    return m_app;
}

AppObject::AppObject(QObject *object)
    :QObject(object)
{

    languageMap_ts["en"] = u8"英语";
    languageMap_ts["zh_CN"] = u8"中文简体";
    languageMap_ts["zh_TW"] = u8"中文繁体";
    languageMap_ts["zh_HK"] = u8"中文繁体";
    languageMap_ts["es"] = u8"西班牙语";
    languageMap_ts["pl"] = u8"波兰语";
    languageMap_ts["ja"] = u8"日语";
    languageMap_ts["de"] = u8"德语";
    languageMap_ts["ko"] = u8"韩语";
    languageMap_ts["it"] = u8"意大利语";
    languageMap_ts["fr"] = u8"法语";
    languageMap_ts["ru"] = u8"俄语";
    languageMap_ts["pt"] = u8"葡萄牙语";
    languageMap_ts["pt_BR"] = u8"葡萄牙语";
    languageMap_ts["fi"] = u8"芬兰语";
    languageMap_ts["vi"] = u8"越南语";
//    languageMap_ts["tr"] = u8"土耳其语";
    languageMap_ts["th"] = u8"泰语";
    languageMap_ts["hu"] = u8"匈牙利语";
    languageMap_ts["sv"] = u8"瑞典语";
    //languageMap_ts["sk"] = u8"斯洛伐克语";
    languageMap_ts["ro"] = u8"罗马尼亚语";
//    languageMap_ts["ms"] = u8"马来语";

    languageMap_ts["ar_sa"] = u8"阿拉伯语";
    languageMap_ts["ar_iq"] = u8"阿拉伯语";
    languageMap_ts["ar_eg"] = u8"阿拉伯语";
    languageMap_ts["ar_ly"] = u8"阿拉伯语";
    languageMap_ts["ar_dz"] = u8"阿拉伯语";
    languageMap_ts["ar_ma"] = u8"阿拉伯语";
    languageMap_ts["ar_tn"] = u8"阿拉伯语";
    languageMap_ts["ar_om"] = u8"阿拉伯语";
    languageMap_ts["ar_ye"] = u8"阿拉伯语";
    languageMap_ts["ar_sy"] = u8"阿拉伯语";
    languageMap_ts["ar_jo"] = u8"阿拉伯语";
    languageMap_ts["ar_lb"] = u8"阿拉伯语";
    languageMap_ts["ar_kw"] = u8"阿拉伯语";
    languageMap_ts["ar_ae"] = u8"阿拉伯语";
    languageMap_ts["ar_bh"] = u8"阿拉伯语";
    languageMap_ts["ar_qa"] = u8"阿拉伯语";

    languageMap_ts["af"] = u8"荷兰语";//南非
    languageMap_ts["nl"] = u8"荷兰语";//标准
    languageMap_ts["nl_be"] = u8"荷兰语";//比利时

    languageMap_ts["et"] = u8"爱沙尼亚语";
    languageMap_ts["bg"] = u8"保加利亚语";
    languageMap_ts["cs"] = u8"捷克语";
    languageMap_ts["da"] = u8"丹麦语";
    languageMap_ts["el"] = u8"希腊语";



    // 添加各种语言对应的键值对，键为显示的语言名称，值为对应的语言代码
    languageMap_baidu.insert(u8"英语", "en");
    languageMap_baidu.insert(u8"中文简体", "zh");
    languageMap_baidu.insert(u8"粤语", "yue");
    languageMap_baidu.insert(u8"文言文", "wyw");
    languageMap_baidu.insert(u8"日语", "jp");
    languageMap_baidu.insert(u8"韩语", "kor");
    languageMap_baidu.insert(u8"法语", "fra");
    languageMap_baidu.insert(u8"西班牙语", "spa");
    languageMap_baidu.insert(u8"泰语", "th");
    languageMap_baidu.insert(u8"阿拉伯语", "ara");
    languageMap_baidu.insert(u8"俄语", "ru");
    languageMap_baidu.insert(u8"葡萄牙语", "pt");
    languageMap_baidu.insert(u8"德语", "de");
    languageMap_baidu.insert(u8"意大利语", "it");
    languageMap_baidu.insert(u8"希腊语", "el");
    languageMap_baidu.insert(u8"荷兰语", "nl");
    languageMap_baidu.insert(u8"波兰语", "pl");
    languageMap_baidu.insert(u8"保加利亚语", "bul");
    languageMap_baidu.insert(u8"爱沙尼亚语", "est");
    languageMap_baidu.insert(u8"丹麦语", "dan");
    languageMap_baidu.insert(u8"芬兰语", "fin");
    languageMap_baidu.insert(u8"捷克语", "cs");
    languageMap_baidu.insert(u8"罗马尼亚语", "rom");
    languageMap_baidu.insert(u8"斯洛文尼亚语", "slo");
    languageMap_baidu.insert(u8"瑞典语", "swe");
    languageMap_baidu.insert(u8"匈牙利语", "hu");
    languageMap_baidu.insert(u8"中文繁体", "cht");
    languageMap_baidu.insert(u8"越南语", "vie");
    languageMap_baidu.insert(u8"其他", "other");
}
