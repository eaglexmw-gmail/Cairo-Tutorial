#ifndef CCANVAS_H
#define CCANVAS_H

#include <gtkmm.h>
#include <gtkmm/drawingarea.h>

#include <vector>
#include "typesNmath.h"




using CairoCtx = Cairo::RefPtr<Cairo::Context> const &;


// drawing primitives
inline void LineWidth( CairoCtx cr, SLineWidth const & w, double const & s=1 )
        {
        cr->set_line_width(w.w / ((s==0)?1:s));
        }

inline void Color( CairoCtx cr, SColor const & c )
        {
        cr->set_source_rgb( c.r, c.g, c.b );
        }

inline void Color( CairoCtx cr, SColor const & c, double const & a )
        {
        cr->set_source_rgba( c.r, c.g, c.b, a );
        }

inline void Color( CairoCtx cr, double const & r, double const & g, double const & b )
        {
        cr->set_source_rgb( r, g, b );
        }

inline void Color( CairoCtx cr, double const & r, double const & g, double const & b, double const & a )
        {
        cr->set_source_rgba( r, g, b, a );
        }

template<typename P>
    void MoveTo(CairoCtx cr, P const & tPoint)
        {
        cr->move_to(tPoint.x, tPoint.y);
        }

template<typename P>
    void LineTo(CairoCtx cr, P const & tPoint)
        {
        cr->line_to(tPoint.x, tPoint.y);
        }

template<typename S, typename... P>
    void Line(CairoCtx cr, S const & tPoint1, P const &... tPoints )
        {
         MoveTo(cr, tPoint1);
        (LineTo(cr, tPoints), ...);
        cr->stroke();
        }

inline void Line(CairoCtx cr, SLine const & tLine )
        {
        MoveTo(cr, tLine.a);
        LineTo(cr, tLine.b);
        cr->stroke();
        }

template<typename C>
    void LineStrip(CairoCtx cr, C const & tPoints )
        {
        bool b{true};
        for ( auto const & a:tPoints )
            {
            if (b)
                {
                MoveTo(cr, a);
                b = false;
                }
            else
                {
                LineTo(cr, a);
                }
            }
        cr->stroke();
        }

template<typename S, typename... P>
    void Polygon(CairoCtx cr, S const & tPoint1, P const &... tPoints )
        {
         MoveTo(cr, tPoint1);
        (LineTo(cr, tPoints), ...);
         LineTo(cr, tPoint1);
        cr->fill();
        }

template<typename C>
    void Polygon(CairoCtx cr, C const & tPoints )
        {
        bool b{true};
        for ( auto const & a:tPoints )
            {
            if (b) MoveTo(cr, a); else LineTo(cr, a);
            }
        cr->fill();
        }

template<typename P>
    void Circle(CairoCtx cr, P const & tPoint, double const & dRadius )
        {
        cr->arc(tPoint.x, tPoint.y, dRadius, 0, 2*M_PI);
        cr->fill();
        }

template<typename P>
    void Ring(CairoCtx cr, P const & tPoint, double const & dRadius )
        {
        cr->arc(tPoint.x, tPoint.y, dRadius, 0, 2*M_PI);
        cr->stroke();
        }

template<typename R>
    void Rectangle(CairoCtx cr, R const & tRect )
        {
        cr->rectangle(tRect.p.x, tRect.p.y, tRect.s.x, tRect.s.y);
        cr->fill();
        }

template<typename P, typename S>
    void Rectangle(CairoCtx cr, P const & tPoint, S const & tSize )
        {
        cr->rectangle(tPoint.x, tPoint.y, tSize.x, tSize.y);
        cr->fill();
        }

template<typename R>
    void Frame(CairoCtx cr, R const & tRect )
        {
        cr->rectangle(tRect.p.x, tRect.p.y, tRect.s.x, tRect.s.y);
        cr->stroke();
        }

template<typename P, typename S>
    void Frame(CairoCtx cr, P const & tPoint, S const & tSize )
        {
        cr->rectangle(tPoint.x, tPoint.y, tSize.x, tSize.y);
        cr->stroke();
        }


inline SPoint Text( CairoCtx cr,
                    SPoint const & pos,
                    std::string const & crsText, 
                    double const & dScale = 1.0,
                    bool const & label = false,
                    int const & nOrintation = TEXT_MIDDLE )
    {
    cr->save();

    Pango::FontDescription font;

    font.set_family("Sans");
    font.set_absolute_size(12.0 * PANGO_SCALE*dScale);

    Gtk::DrawingArea w;
    auto layout = w.create_pango_layout(crsText);

    layout->set_font_description(font);
    int iWidth {0};
    int iHeight{0};
    SPoint tSize{.0,.0};

    layout->get_pixel_size(iWidth, iHeight);
    tSize = SPoint{iWidth, iHeight};

    SPoint tPos;
    switch (nOrintation)
        {
        case TEXT_LEFT:   tPos = {pos.x          , pos.y}; break;
        case TEXT_MIDDLE: tPos = {pos.x-tSize.x/2, pos.y}; break;
        case TEXT_RIGHT:  tPos = {pos.x-tSize.x  , pos.y}; break;
        }

    if (label)
        {
        cr->save();
        Color(cr, GRAY, .75);
        LineWidth(cr, {.0+iHeight});
        Line(cr, SLine{{tPos.x        +tSize.y/4, tPos.y},
                       {tPos.x+tSize.x-tSize.y/4, tPos.y}});    
        cr->restore();
        }

    switch (nOrintation)
        {
        case TEXT_LEFT:   cr->move_to(pos.x          , pos.y-tSize.y/2); break;
        case TEXT_MIDDLE: cr->move_to(pos.x-tSize.x/2, pos.y-tSize.y/2); break;
        case TEXT_RIGHT:  cr->move_to(pos.x-tSize.x  , pos.y-tSize.y/2); break;
        }

    layout->show_in_cairo_context(cr);
    cr->restore();
    
    return std::move(tSize);
    }


using VBricks = std::vector<SRect>;



class CCanvas : public Gtk::DrawingArea
    {
    public:

        SRect   m_tPaddle;
        SPoint  m_tBall;
        bool    m_bBallFree{false};
        VBricks m_vBricks;
        
        bool    m_bBricksInitialized{false};
        SRect   m_tPlayground{{0,0},{1,1}};

        bool m_bButtonDown{false};

        void AnimateHlt()    { m_bAnimate  = !m_bAnimate; }
        void AnimateAdd()    { m_dAnimate *= 1.1; m_dAnimate = (m_dAnimate>m_dAnimateMax)?m_dAnimateMax:m_dAnimate; }
        void AnimateSub()    { m_dAnimate *= 0.9; m_dAnimate = (m_dAnimate<m_dAnimateMin)?m_dAnimateMin:m_dAnimate; }
        void AnimateRev()    { m_bAnimateLeft = !m_bAnimateLeft; }

        
        CCanvas()
            {
            add_events(Gdk::BUTTON_PRESS_MASK | Gdk::SCROLL_MASK | Gdk::SMOOTH_SCROLL_MASK);
            add_events(Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);
            add_events(Gdk::BUTTON1_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::POINTER_MOTION_MASK);
            add_events(Gdk::KEY_PRESS_MASK | Gdk::KEY_RELEASE_MASK);

            m_fSlot         = sigc::bind(sigc::mem_fun(*this, &CCanvas::Animate), 0);
            m_afConnections = Glib::signal_timeout().connect(m_fSlot, 20);

            for ( int i{0}; i<13; ++i)
                {
                auto constexpr bs {38.0};  // button size
                auto constexpr uix{20.0},uiy{20.0},uiw{bs},uih{bs};
                auto constexpr bsp{ 8.0};  // button spacing
                auto constexpr bbo{16.0};  // button bar offset
                m_voButtons.emplace_back( bbo+i*(uiw+bsp), bbo+uiy, uiw, uih,
                                          std::to_string(i),
                                          std::to_string(i) );
                }
            }

        virtual ~CCanvas()
            {
            m_afConnections.disconnect();
            };

    protected:
        // Override default signal handler:
        bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
        bool on_scroll_event(GdkEventScroll *event) override;
        bool on_button_press_event(GdkEventButton * event) override;
        bool on_motion_notify_event(GdkEventMotion *event) override;
        bool on_button_release_event(GdkEventButton* release_event) override;

        bool Collision(SPoint const & tPoint);
        
        SPoint   m_tCtxSize     { .0,.0 };
        double   m_dScale       { 1.0 };
        
        bool     m_bShiftInit   { true };
        SPoint   m_tShift       { .0,.0 };
        SPoint   m_tEventPress  { .0,.0 };
        SPoint   m_tEventRelease{ .0,.0 };
        SPoint   m_tShiftStart  { .0,.0 };

       
        SPoint   m_tMousePos;

    public:

        struct SCollision
            {
            double d       {  1e9  };
            double dAngle  {  0.0  };
            SPoint tWhere  { .0,.0 };
            SPoint tOffset { .0,.0 };
            enum class EWhat
                {
                none,
                Element
                } eWhat { EWhat::none };
            size_t nIndex { 0 };
            size_t nSubIx { 0 };
            } m_tCollision;

    protected:

        // animation
        bool             m_bAnimate{true};
        bool             m_bAnimateLeft{true};

        // animation clock
        double           m_dAnimator{0}; // $m_tAnimator animation parameter
        double           m_dAnimStep{0}; // intermediate animation parameter
        sigc::slot<bool> m_fSlot;
        sigc::connection m_afConnections;

        double           m_dAnimate   {0.0025}; // animation steps width
        double const     m_dAnimateMax{0.0250}; // maximal animation step width
        double const     m_dAnimateMin{0.0025}; // minimal animation step width
/*
        double m_nTick{.0};
        bool Animate(int c) // vibration
            {
            if (!m_bAnimate) return true;
            m_dAnimator = (m_dAnimator > 0) ? 0 : 1;
            queue_draw();
            return true;
            }
*/
        bool Animate(int c) // rotation
            {
            if (!m_bAnimate) return true;
            if (m_bAnimateLeft)
                m_dAnimator = (m_dAnimator <=  m_dAnimate) ? 1 : m_dAnimator-m_dAnimate;
                else
                m_dAnimator = (m_dAnimator >=1-m_dAnimate) ? 0 : m_dAnimator+m_dAnimate;
            queue_draw();
            return true;
            }
/*
        bool Animate(int c) // bidirectional
            {
            if (!m_bAnimate) return true;
            if (m_bAnimateLeft)
                m_dAnimator -= m_dAnimate;
                else
                m_dAnimator += m_dAnimate;
            if (m_dAnimator <= 0) m_bAnimateLeft = false;
            if (m_dAnimator >= 1) m_bAnimateLeft = true;
            queue_draw();
            return true;
            }
*/
    VButtons    m_voButtons;            // button bar
    std::string m_oButtonPressed{""};   // last pressed button

    }; // CCanvas

// CCANVAS_H
#endif
