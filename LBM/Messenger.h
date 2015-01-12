#ifndef INCLUDED_LBM_MESSENGER
#define INCLUDED_LBM_MESSENGER

class Messenger
{
    size_t d_p;
    double d_src;
    double *d_dest;

    size_t d_dir;

    public:
        Messenger(size_t processor, size_t dir = 0)
        :
            d_p(processor),
            d_dir(dir)
        {}

        ~Messenger() {}

        double *source()
        {
            return &d_src;
        }
        double *dest()
        {
            return d_dest;
        }
        void dest(double *dest)
        {
            d_dest = dest;
        }


};

#endif