#include <FL/Fl_Gl_Window.H>
#include <FL/gl.h>

class Fltk13WorldMap : public Fl_Gl_Window {
    void RectLine(int x1, int y1, int x2, int y2) {
        glBegin(GL_LINE_LOOP);
        glVertex2s(x1, y1); glVertex2s(x2, y1);
        glVertex2s(x2, y2); glVertex2s(x1, y2);
        glEnd();
    }
protected:
    void draw() {
        if ( !valid() ) {
            // First time? init viewport, etc.
            valid(1);
            glLoadIdentity();
            glViewport(0, 0, w(), h());
            glOrtho(-w(), w(), -h(), h(), -1, 1);
        }
        // Clear screen
        glClear(GL_COLOR_BUFFER_BIT);
        // Draw white 'X'
        glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_LINE_STRIP); glVertex2f(w(),  h()); glVertex2f(-w(), -h()); glEnd();
        glBegin(GL_LINE_STRIP); glVertex2f(w(), -h()); glVertex2f(-w(),  h()); glEnd();
        // Draw yellow border last, around the outer edge
        glColor3f(1.0, 1.0, 0.0);
        RectLine(-w()+1, -h()+1, w()-1, h()-1);
    }
public:
    Fltk13WorldMap(int x, int y, int w, int h, char* l=0) : Fl_Gl_Window(x, y, w, h, l) {
        end();
    }
};