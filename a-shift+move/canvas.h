#include <gtkmm.h>
#include <gtkmm/drawingarea.h>

#include <vector>


struct SPoint
    {
    SPoint() = default;
    SPoint(SPoint const & src) = default;
    template<typename T>
        SPoint(T const & x, T const & y) : x(x), y(y) {}
    template<typename T>
        SPoint(T const & t) : x(t.x), y(t.y) {}
    double x{0}, y{0};
    template<typename P>
        SPoint operator = (P const & p)
            {
            x = p.x;
            y = p.y;
            return *this;
            }
    template<typename P>
        SPoint operator -= (P const & p)
            {
            x -= p.x;
            y -= p.y;
            return *this;
            }
    template<typename P>
        SPoint operator += (P const & p)
            {
            x += p.x;
            y += p.y;
            return *this;
            }
    };

inline bool operator == (SPoint const & p1, SPoint const & p2)
    {
    return (p1.x==p2.x) && (p1.y==p2.y);
    }

inline bool operator != (SPoint const & p1, SPoint const & p2)
    {
    return !(p1==p2);
    }

inline SPoint operator - (SPoint const & p1, SPoint const & p2)
    {
    return {p1.x-p2.x, p1.y-p2.y};
    }

inline SPoint operator + (SPoint const & p1, SPoint const & p2)
    {
    return {p2.x+p1.x, p2.y+p1.y};
    }

inline SPoint operator / (SPoint const & p, double const & d)
    {
    return {p.x/d, p.y/d};
    }

inline SPoint operator * (SPoint const & p, double const & d)
    {
    return {p.x*d, p.y*d};
    }


struct SFleck
    {
    double x{0}, y{0}, r{0};
    template<typename P>
        SFleck operator -= (P const & p)
            {
            x -= p.x;
            y -= p.y;
            return *this;
            }
    template<typename P>
        SFleck operator += (P const & p)
            {
            x += p.x;
            y += p.y;
            return *this;
            }
    template<typename P>
        SFleck operator = (P const & p)
            {
            x = p.x;
            y = p.y;
            return *this;
            }
   };

struct SColor
    {
    double r{0},g{0},b{0};
    };

using VPoints  = std::vector<SPoint>;
using VFlecken = std::vector<SFleck>;

// the first argument can be anything having the members x and y
// a Gtk event, a SFleck or whatever
template<typename P, typename T>
    double Distance( P const & a, T const & b )
        {
        return sqrt( pow((a.x-b.x),2) + pow((a.y-b.y),2) );
        }


class CCanvas : public Gtk::DrawingArea
    {
    public:
        CCanvas()
            {
            add_events(Gdk::BUTTON_PRESS_MASK | Gdk::SCROLL_MASK | Gdk::SMOOTH_SCROLL_MASK);
            add_events(Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);
            add_events(Gdk::BUTTON1_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::POINTER_MOTION_MASK);
            add_events(Gdk::KEY_PRESS_MASK | Gdk::KEY_RELEASE_MASK);
            }

        virtual ~CCanvas() { };

    protected:
        // Override default signal handler:
        bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
        bool on_scroll_event(GdkEventScroll *event) override;
        bool on_button_press_event(GdkEventButton * event) override;
        bool on_motion_notify_event(GdkEventMotion *event) override;
        bool on_button_release_event(GdkEventButton* release_event) override;

        bool Collision(SPoint const & tPoint);
        
        bool     m_bShiftInit   { true };
        SPoint   m_tShift       { .0,.0 };
        SPoint   m_tEventPress  { .0,.0 };
        SPoint   m_tEventRelease{ .0,.0 };
        SPoint   m_tShiftStart  { .0,.0 };

       
        SPoint   m_tMousePos;
        SColor   m_tMouseColor{ .5,.5,.5 };
        VPoints  m_vMouseTrail;
        VFlecken m_vFlecken { {30,30,20}, {300,300,50}, {500,200,40},
                              {40,50,25}, {240,320,30}, {580,270,45} };

        struct SCollision
            {
            SPoint tWhere { .0,.0 };
            SPoint tOffset{ .0,.0 };
            enum class EWhat
                {
                none,       // there was no collision
                Fleck,      // move a Fleck
                Line,       // move a Line
                }eWhat {EWhat::none};
            int nIndex {0};	// O: L1, L2, L3
            int nSubIx {0};	// L: P1, PM, P2
            } m_tCollision;

    }; // CCanvas
