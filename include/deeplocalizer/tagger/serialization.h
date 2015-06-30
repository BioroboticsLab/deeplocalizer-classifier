
#ifndef DEEP_LOCALIZER_SERIALIZATION_H
#define DEEP_LOCALIZER_SERIALIZATION_H

#include <boost/serialization/string.hpp>
#include <QString>

namespace boost {
    namespace serialization {

        template<class Archive>
        void save( Archive & ar, const QString& qStringParam, const unsigned int )
        {
            // save class member variables
            std::string stdString = qStringParam.toStdString();
            ar << BOOST_SERIALIZATION_NVP(stdString);
        }

        template<class Archive>
        void load( Archive & ar, QString& qStringParam, const unsigned int )
        {
            // load class member variables
            std::string stdString;
            ar >> BOOST_SERIALIZATION_NVP(stdString);
            qStringParam = qStringParam.fromStdString(stdString);
        }

        template<class Archive>
        void serialize(Archive & ar, QString & t, const unsigned int file_version)
        {
            split_free(ar, t, file_version);
        }

        template<class Archive, class Allocator, class T>
        inline void save(Archive& ar, const std::deque<std::unique_ptr<T>, Allocator>& t,
                const unsigned int /* file_version */){
            // record number of elements
            collection_size_type count (t.size());
            ar << BOOST_SERIALIZATION_NVP(count);

            for(const auto& elt: t)
                ar << boost::serialization::make_nvp("item", elt);
        }

        template<class Archive, class Allocator, class T>
        inline void load(Archive & ar, std::deque<std::unique_ptr<T>, Allocator>& t,
                const unsigned int /* file_version */){
            // retrieve number of elements
            collection_size_type count;
            ar >> BOOST_SERIALIZATION_NVP(count);
            t.clear();
            while(count-- > 0){
                std::unique_ptr<T> i;
                ar >> boost::serialization::make_nvp("item", i);
                t.push_back(std::move(i));// move object
            }
        }

        template<class Archive, class Allocator, class T>
        inline void serialize(Archive & ar, std::deque<std::unique_ptr<T>, Allocator>& t,
                const unsigned int file_version){
            boost::serialization::split_free(ar, t, file_version);
        }

    } // namespace serialization
} // namespace boost
#endif //DEEP_LOCALIZER_SERIALIZATION_H
