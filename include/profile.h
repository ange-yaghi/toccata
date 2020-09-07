#ifndef TOCCATA_UI_PROFILE_H
#define TOCCATA_UI_PROFILE_H

#include "setting.h"

#include "delta.h"

#include <string>

namespace toccata {

    class Profile {
    public:
        Profile();
        ~Profile();

        virtual bool GetSetting(const std::string &name, ysVector *v);
        virtual bool GetSetting(const std::string &name, std::string *v);
        virtual bool GetSetting(const std::string &name, bool *v);
        virtual bool GetSetting(const std::string &name, int *v) ;
        virtual bool GetSetting(const std::string &name, double *v);

    protected:
        virtual Setting *GetSetting(const std::string &name) const = 0;
    };

} /* namespace toccata */

#endif /* TOCCATA_UI_PROFILE_H */
