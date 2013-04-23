#ifndef IMAGEPAINTER_H
#define IMAGEPAINTER_H
#include <cstdlib>
#include <QPixmap>

class QPainter;


namespace QtAddons {

class ImagePainter
{
public:
    ImagePainter();
    ~ImagePainter();
    void Render(const QPainter &painter, int x, int y, int w, int h);

    void set_image(float const * const data, size_t const * const dims);
    void set_image(float const * const data, size_t const * const dims, const float low, const float high);
//	void set_plot(PlotData data, int idx);
    int clear_plot(int idx=-1);
//	void set_rectangle(ImageViewerRectangle rect, int idx);
    int clear_rectangle(int idx=-1);

    int clear();
    void set_levels(const float level_low, const float level_high);
    void get_levels(float *level_low, float *level_high);
    void get_image_minmax(float *level_low, float *level_high);
    void show_clamped(bool show);

    //void set_interpolation(Gdk::InterpType interp) {m_Interpolation=interp;}
protected:
    void prepare_pixbuf();

    int m_dims[2];
    int m_NData;

    float m_ImageMin;
    float m_ImageMax;

    float m_MinVal;
    float m_MaxVal;

    float m_WindowScale;
    int offset_x;
    int offset_y;
    int scaled_width;
    int scaled_height;

    //Gdk::InterpType m_Interpolation;
    float * m_data;  //<! float pixel buffer
    //guint8 * m_cdata;   //<! RGB Pixel buffer

//	  std::map<int,ImageViewerRectangle> m_BoxList;
//	  std::map<int,PlotData> m_PlotList;

      QPixmap pixmap;
};

}

#endif // IMAGEPAINTER_H
