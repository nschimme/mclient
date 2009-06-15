#ifndef MCLIENTEVENTDATA_H
#define MCLIENTEVENTDATA_H

#include <QSharedData>

class QVariant;


class MClientEventData : public QSharedData {
    
    public:
        MClientEventData(QVariant* payload);
	MClientEventData(const MClientEventData& med);
        ~MClientEventData();

        QVariant* payload();
        
        // testing
        void refs() const;

    private:
        QVariant* _payload;

};


#endif /* MCLIENTEVENTDATA_H */
