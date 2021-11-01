/* date = March 20th 2021 5:33 pm */

#ifndef JUNK_H
#define JUNK_H

//This will temp hold code that isn't being used
//currently making rasterizer and don't want this to
//clutter main

internal int32
abs_int32(int32 value)
{
    if(value < 0)
    {
        return -value;
    }
    
    return value;
}

internal float64
abs_float64(float64 value)
{
    if(value < 0)
    {
        return -value;
    }
    
    return value;
}

internal int32
floor_float64(float64 value)
{
    if(value < 0)
    {
        value = (int32)(value-0.5f);
    }
    else
    {
        value = (int32)value;
    }
    
    return value;
}

internal float32
linear_interpolation(float32 v0, float32 v1, float32 t)
{
    return (1-t)*v0 + t*v1;
}

internal int32
_linear_interpolation(int32 v0, int32 v1, float32 t)
{
    return (int32)((1-t)*v0 + t*v1);
}


// TODO(Ajeej): Test to see if you should pass vect4 as a pointer
internal void
draw_point(framebuffer_t *framebuffer, int32 x, int32 y, fvector4_t color)
{
    // TODO(Ajeej): Check for x and y being out of bounds
    int32 index = (y * framebuffer->width + x) * 4;
    
    // TODO(Ajeej): Swap blue and red when implementing surface
    uint8 red = (uint8)linear_interpolation(0.0f, 255.0f, color.x);
    uint8 green = (uint8)linear_interpolation(0.0f, 255.0f, color.y);
    uint8 blue = (uint8)linear_interpolation(0.0f, 255.0f, color.z);
    uint8 alpha = (uint8)linear_interpolation(0.0f, 255.0f, color.w);
    
    // TODO(Ajeej): See if this step can be taken out
    uint8 _color[4] = { blue, green, red, alpha };
    
    if(x > 0 && x < framebuffer->width &&
       y > 0 && y < framebuffer->height)
    {
        for(uint32 channel = 0; channel < 4; channel++)
        {
            framebuffer->color_buffer[index + channel] = _color[channel];
        }
    }
}

internal void
draw_line(framebuffer_t *framebuffer, int32 x0, int32 y0, int32 x1, int32 y1, fvector4_t color)
{
    int32 dx = abs_int32(x1 - x0);
    int32 dy = -abs_int32(y1 - y0); //dy will be used as a negative
    int32 xstep = (x0 < x1) ? 1 : -1;
    int32 ystep = (y0 < y1) ? 1 : -1;
    int32 error = dx+dy, double_error;
    
    for(;;)
    {
        draw_point(framebuffer, x0, y0, color);
        double_error = 2 * error;
        if(double_error >= dy)
        {
            if(x0 == x1) break;
            error += dy;
            x0 += xstep;
        }
        
        if(double_error <= dx)
        {
            if(y0 == y1) break;
            error += dx;
            y0 += ystep;
        }
    }
    
}

internal void
draw_square(framebuffer_t *framebuffer,
            int32 x0, int32 y0,
            int32 x1, int32 y1,
            fvector4_t color)
{
    // TODO(Ajeej): Make this much more optomized
    if(x0 > x1) { int32 temp = x0; x0 = x1; x1 = temp; };
    if(y0 > y1) { int32 temp = y0; y0 = y1; y1 = temp; };
    
    draw_line(framebuffer, x0, y0, x0, y1, color);
    draw_line(framebuffer, x0, y1, x1, y1, color);
    draw_line(framebuffer, x1, y1, x1, y0, color);
    draw_line(framebuffer, x1, y0, x0, y0, color);
}

internal void
draw_ellispe(framebuffer_t *framebuffer,
             int32 center_x, int32 center_y, int32 a, int32 b,
             fvector4_t color)
{
    int32 x = -a, y = 0; // Start from the Quadrant II from bottom left to top right
    
    int64 dy = x*x, dx = dy + (1 + x*2)*((int64)b*b); // TODO(Ajeej): write math in comments as proof
    int64 error = dx + dy, double_error;
    
    do
    {
        // TODO(Ajeej): Upside down if the rendering coords are
        // placed to render from top-down; see if adding surface
        // will fix the issue
        draw_point(framebuffer, (center_x - x), (center_y + y), color); //Create I Quadrant
        draw_point(framebuffer, (center_x + x), (center_y + y), color); //Create II Quadrant
        draw_point(framebuffer, (center_x + x), (center_y - y), color); //Create III Quadrant
        draw_point(framebuffer, (center_x - x), (center_y - y), color); //Create IV Quadrant
        
        double_error = 2*error;
        if(double_error >= (2*x + 1)*(int64)b*b)
        {
            ++x;
            error += dx += 2 * (int64)b*b; 
        }
        if(double_error <= (2*y + 1)*(int64)a*a)
        {
            ++y;
            error += dy += 2 * (int64)a*a;
        }
    } while (x <= 0);
    
    while(y++ < b)
    {
        draw_point(framebuffer, center_x, center_y + y, color);
        draw_point(framebuffer, center_x, center_y - y, color);
    }
}

internal void 
draw_circle(framebuffer_t *framebuffer, 
            int32 center_x, int32 center_y, int32 r,
            fvector4_t color)
{
    int x = -r, y = 0, error = 2 - r*2;
    
    do {
        draw_point(framebuffer, center_x-x, center_y+y, color);
        draw_point(framebuffer, center_x-y, center_y-x, color);
        draw_point(framebuffer, center_x+x, center_y-y, color);
        draw_point(framebuffer, center_x+y, center_y+x, color);
        
        r = error;
        if(r <= y) //if(2*error < 2*y + 1)
        {
            error += ++y*2+1;
        }
        if(r > x || error > y)
        {
            error += ++x*2+1;
        }
        
    } while (x < 0);
}

internal void
draw_ellipse_rect(framebuffer_t *framebuffer,
                  int32 x0, int32 y0,
                  int32 x1, int32 y1,
                  fvector4_t color)
{
    int64 a = abs_int32(x1 - x0), b = abs_int32(y1 - y0), offset = b&1;
    float64 dx = 4*(1-a)*b*b, dy = 4*(offset+1)*a*a;
    float64 error = dx + dy + offset*a*a, double_error;
    
#if DEBUG_RENDERING
    
    fvector4_t white = { 1.0f, 1.0f, 1.0f, 1.0f };
    draw_square(framebuffer, x0 - 1, y0 - 1, x1 + 1, y1 + 1, white);
    
#endif
    
    // TODO(Ajeej): Replace with swap function
    if(x0 > x1) { x0 = x1; x1 += a; }
    if(y0 > y1) { y0 = y1; }
    y0 += (b+1)*(0.5f);
    y1 = y0-offset;
    int64 increment_x = 8*b*b, increment_y = 8*a*a;
    
    do {
        
        draw_point(framebuffer, x1, y0, color);
        draw_point(framebuffer, x0, y0, color);
        draw_point(framebuffer, x0, y1, color);
        draw_point(framebuffer, x1, y1, color);
        double_error = 2*error;
        
        if(double_error <= dy) { y0++; y1--; error += dy += increment_y; }
        if(double_error >= dx || 2*error > dy) { x0++; x1--; error += dx += increment_x; }
        
    } while (x0 <= x1);
    
    while (y0 - y1 < b)
    {
        draw_point(framebuffer, x0-1, y0, color);
        draw_point(framebuffer, x1+1, y0++, color);
        draw_point(framebuffer, x0-1, y1, color);
        draw_point(framebuffer, x1+1, y1--, color);
    }
    
}

// TODO(Ajeej): Delete this
internal void
test_square_bcurve(framebuffer_t *framebuffer,
                   int32 x0, int32 y0,
                   int32 x1, int32 y1,
                   int32 x2, int32 y2,
                   uint32 step_count,
                   fvector4_t color)
{
    float32 step = 1.0f/step_count;
    for(float32 t = 0; t <= 1; t+=step)
    {
        float32 a = (1-t);
        float32 b = t;
        float32 a2 = a*a;
        float32 b2 = b*b;
        int32 x = 0;
        int32 y = 0;
        
        x = x0*a2 + 2*x1*a*b + x2*b2;
        y = y0*a2 + 2*y1*a*b + y2*b2;
        
        draw_point(framebuffer, x, y, color);
    }
    
#if DEBUG_RENDERING
    
    fvector4_t white = { 1.0f, 1.0f, 1.0f, 1.0f };
    uint32 radius = 3;
    
    draw_circle(framebuffer, x0, y0, radius, white);
    draw_circle(framebuffer, x1, y1, radius, white);
    draw_circle(framebuffer, x2, y2, radius, white);
    draw_line(framebuffer, x0, y0, x1, y1, white);
    draw_line(framebuffer, x1, y1, x2, y2, white);
    
#endif
}

// TODO(Ajeej): Replace for rational bezier
internal void
draw_quad_bezier_segment(framebuffer_t *framebuffer,
                         int32 x0, int32 y0,
                         int32 x1, int32 y1,
                         int32 x2, int32 y2,
                         fvector4_t color)
{
    int32 sx = x2-x1, sy = y2-y1;
    int64 xx = x0-x1, yy = y0-y1, xy;
    float64 dx, dy, err, cur = xx*sy-yy*sx;
    
    /*The sign of the gradient can not change*/
    ASSERT((xx*sx <= 0 && yy*sy <= 0));
    
    /*Begin with longer section*/
    if(sx*(int64)sx+sy*(int64)sy > xx*xx + yy*yy)
    {
        x2 = x0; x0 = sx+x1; y2 = y0; y0 = sy+y1; cur = -cur;
    }
    
    /*Not a straign line*/
    if(cur != 0)
    {
        xx += sx; xx *= sx = x0 < x2 ? 1 : -1; /*x-step direction*/
        yy += sy; yy *= sy = y0 < y2 ? 1 : -1; /*y-step direction*/
        xy = 2*xx*yy; xx *= xx; yy *= yy; /*differences 2nd degree*/
        if(cur*sx*sy < 0) /*negates curve if needed*/
        {
            xx = -xx; yy = -yy; xy = -xy; cur = -cur;
        }
        
        dx = 4.0f*sy*cur*(x1-x0)+xx-xy; /*difference 1st degree*/
        dy = 4.0f*sx*cur*(y0-y1)+yy-xy;
        xx += xx; yy += yy; err = dx+dy+xy; /*error 1st step*/
        
        do
        {
            draw_point(framebuffer, x0, y0, color);
            if(x0 == x2 && y0 == y2) return;
            y1 = 2*err < dx; /*save value for test of y step*/
            if(2*err > dy) { x0 += sx; dx -= xy; err += dy += yy; } /*x step*/
            if(y1) { y0 +=sy; dy-= xy; err += dx += xx; } /*y-step*/
        } while (dy < dx); /*until gradient negates*/
    }
    
    draw_line(framebuffer, x0, y0, x2, y2, color);
}

internal void
draw_quad_bezier(framebuffer_t *framebuffer,
                 int32 x0, int32 y0,
                 int32 x1, int32 y1,
                 int32 x2, int32 y2,
                 fvector4_t color)
{
    int32 x = x0-x1, y = y0-y1;
    float64 t = x0-2*x1+x2, r;
    
    if((int64)x*(x2-x1) > 0) /*Horizatonal tangent*/
    {
        if((int64)y*(y2-y1) > 0) /*Vertical tangent*/
        {
            /*Which tangent came first*/
            if(abs_float64((y0-2*y1+y2)/t*x) > abs_int32(y))
            {
                /*Swap so horizontal tangent is first*/
                x0 = x2; x2 = x+x1; y0 = y2; y2 = y+y1;
            }
        }
        
        t = (x0-x1)/t;
        r = (1-t)*((1-t)*y0+2.0f*t*y1)+t*t*y2; /*Get y of P4*/
        t = (x0*x2-x1*x1)*t/(x0-x1); /*Get x of P4*/
        /*x and y of P4*/
        x = floor_float64(t+0.5f); y = floor_float64(r+0.5f);
        /*Get y of P3 from line intersecting line of P0-P1*/
        r = (y1-y0)*(t-x0)/(x1-x0)+y0; 
        
        draw_quad_bezier_segment(framebuffer,
                                 x0, y0,
                                 x, floor_float64(r+0.5f),
                                 x, y,
                                 color);
        /*Get y of P8 from intersection of line though
  P4 and the line between P1-P2*/
        r = (y1-y2)*(t-x2)/(x1-x2)+y2;
        
        /*P0 = P4 and P1 = P8, moving up the curve*/
        x0 = x1 = x; y0 = y; y1 = floor_float64(r+0.5f);
    }
    
    if((int64)(y0-y1)*(y2-y1) > 0) /*vertical cut at P6*/
    {
        t = y0-2*y1+y2; t = (y0-y1)/t; 
        /*Get the x value of P6 form ty*/
        r = (1-t)*((1-t)*x0+2.0f*t*x1)+t*t*x2;
        /*Get the y value of P5 which is the same as P6's*/
        t = (y0*y2-y1*y1)*t/(y0-y1);
        x = floor_float64(r+0.5f); y = floor_float64(t+0.5f);
        /*Get the x value of P5 from the intersection of the
line through P6 and the line between P1-P2*/
        r = (x1-x0)*(t-y0)/(y1-y0)+x0;
        draw_quad_bezier_segment(framebuffer,
                                 x0, y0,
                                 floor_float64(r+0.5f), y,
                                 x, y,
                                 color);
        
        /*Get P7 from the intersection between the line
through P6 and the line between P1-P2*/
        r = (x1-x2)*(t-y2)/(y1-y2)+x2;
        /*P0 = P6 and P1 = P7, moving up a segment*/
        x0 = x; x1 = floor_float64(r+0.5f); y0 = y1 = y;
    }
    
    draw_quad_bezier_segment(framebuffer,
                             x0, y0,
                             x1, y1,
                             x2, y2,
                             color);
}

// TODO(Ajeej): Check performance difference between float literal and double literal
internal void
draw_quad_rational_bezier_segment(framebuffer_t *framebuffer,
                                  int32 x0, int32 y0,
                                  int32 x1, int32 y1,
                                  int32 x2, int32 y2,
                                  float32 w,
                                  fvector4_t color)
{
    int32 sx = x2-x1, sy = y2-y1;
    float64 dx = x0-x2, dy = y0-y2, xx = x0-x1, yy = y0-y1;
    float64 xy = xx*sy+yy*sx, cur = xx*sy-yy*sx, err;
    
    /*Sign of gradient must not change*/
    ASSERT((xx*sx <= 0.0 && yy*sy <= 0.0));
    
    if(cur != 0.0 && w > 0.0) /*not a straight line*/
    {
        if(sx*(int64)sx+sy*(int64)sy > xx*xx+yy*yy)
        {
            /*Swap points in order to start at longest part*/
            x2 = x0; x0 -= dx; y2 = y0; y0 -= dy; cur = -cur;
        }
        xx = 2.0*(4.0*w*sx*xx+dx*dx); /*differences 2nd degree*/
        yy = 2.0*(4.0*w*sy*yy+dy*dy);
        sx = x0 < x2 ? 1 : -1; /*step x*/
        sy = y0 < y2 ? 1 : -1; /*step y*/
        xy = -2.0*sx*sy*(2.0*w*xy+dx*dy);
        
        if(cur*sx*sy < 0.0f)
        {
            /*Swap points if curviture is negative*/
            xx = -xx; yy = -yy; xy = -xy; cur = -cur;
        }
        
        dx = 4.0*w*(x1-x0)*sy*cur+xx/2.0+xy; /*differences 1st degree*/
        dy = 4.0*w*(y0-y1)*sx*cur+yy/2.0+xy;
        
        if(w < 0.5f && dy > dx) /*flat ellipse*/
        {
            cur = (w+1.0)/2.0; w = sqrt(w); xy = 1.0/(w+1.0);
            /*subdivide curve in half*/
            sx = floor_float64((x0+2.0*w*x1+x2)*xy/2.0+0.5);
            sy = floor_float64((y0+2.0*w*y1+y2)*xy/2.0+0.5);
            dx = floor_float64((w*x1+x0)*xy+0.5);
            dy = floor_float64((y1*w+y0)*xy+0.5);
            
            OutputDebugStringA("Flat Ell part 1\n");
            draw_quad_rational_bezier_segment(framebuffer,
                                              x0, y0,
                                              dx, dy,
                                              sx, sy,
                                              cur, color);
            dx = floor_float64((w*x1+x2)*xy+0.5);
            dy = floor_float64((y1*w+y2)*xy+0.5);
            
            OutputDebugStringA("Flat Ell part 2\n");
            draw_quad_rational_bezier_segment(framebuffer,
                                              sx, sy,
                                              dx, dy,
                                              x2, y2,
                                              cur, color);
            return;
        }
        
        err = dx+dy-xy; /*1 step error*/
        do
        {
            draw_point(framebuffer, x0, y0, color);
            if(x0 == x2 && y0 == y2) return;
            
            x1 = 2*err > dy; y1 = 2*(err+yy) < -dy;
            if(2*err < dx || y1)
            {
                y0 += sy; dy += xy; err += dx += xx;
            }
            if(2*err > dx || x1)
            {
                x0 += sx; dx += xy; err += dy += yy;
            }
        } while(dy <= xy && dx >=xy);
    }
    draw_line(framebuffer, x0, y0, x2, y2, color);
}

internal void
draw_quad_rational_bezier(framebuffer_t *framebuffer, 
                          int32 x0, int32 y0, 
                          int32 x1, int32 y1,
                          int32 x2, int32 y2,
                          float32 w,
                          fvector4_t color)
{
    int32 x = x0-2*x1+x2, y = y0-2*y1+y2;
    float64 xx = x0-x1, yy = y0-y1, ww, t, q;
    ASSERT(w >= 0.0);
    
    if(xx*(x2-x1) > 0) /*Horizontal tangent*/
    {
        if(yy*(y2-y1) > 0) /*Vertical tangent*/
            if(abs_float64(xx*y) > abs_float64(yy*x)) {
            /*Swap points so horizontal cut is first*/
            x0 = x2; x2 = xx+x1; y0 = y2; y2 = yy+y1;
            
        }
        if(x0 == x2 || w == 1.0)
        {
            /*The curve is non-rational*/
            t = (x0-x1)/(float64)x;
        }
        else
        {
            /*The curve is rational*/
            q = sqrt(4.0*w*w*(x0-x1)*(x2-x1)+(x2-x0)*(int64)(x2-x0));
            if(x1<x0) q = -q;
            /*Get t at P4*/
            t = (2.0*w*(x0-x1)-x0+x2+q)/(2.0*(1.0-w)*(x2-x0));
        }
        q = 1.0/(2.0*t*(1.0-t)*(w-1.0)+1.0); /*subdivide at t*/
        
        /*Get position of P4*/
        xx = (t*t*(x0-2.0*w*x1+x2)+2.0*t*(w*x1-x0)+x0)*q;
        yy = (t*t*(y0-2.0*w*y1+y2)+2.0*t*(w*y1-y0)+y0)*q;
        /*Get squared weight of P3*/
        ww = t*(w-1.0)+1.0; ww *= ww*q;
        /*Get weight of P8*/
        w = ((1.0-t)*(w-1.0)+1.0)*sqrt(q);
        /*Get rounded position of P4*/
        x = floor_float64(xx+0.5);
        y = floor_float64(yy+0.5);
        /*Get y position of P3 with the intersection of
        the line through P4 and the line between P0-P1*/
        yy = (xx-x0)*(y1-y0)/(x1-x0)+y0;
        
        OutputDebugStringA("Horizontal Tang\n");
        draw_quad_rational_bezier_segment(framebuffer,
                                          x0, y0,
                                          x, floor_float64(yy+0.5),
                                          x, y,
                                          ww, color);
        
        
        /*Get y position of P8 with the intersection of
        the line through P4 and the line between P1-P2*/
        yy = (xx-x2)*(y1-y2)/(x1-x2)+y2;
        /*P0 = P4 and P1 = P8*/
        y1 = floor_float64(yy+0.5); x0 = x1 = x; y0 = y;
    }
    
    if((y0-y1)*(int64)(y2-y1) > 0) /*Vertical tangent*/
    {
        if(y0 == y2 || w == 1.0)
        {
            /*The curve in non-rational*/
            t = (y0-y1)/(y0-2.0*y1+y2);
        }
        else
        {
            /*The curve is rational*/
            q = sqrt(4.0*w*w*(y0-y1)*(y2-y1)+(y2-y0)*(int64)(y2-y0));
            if(y1 < y0) q = -q;
            /*Get t at P6*/
            t = (2.0*w*(y0-y1)-y0+y2+q)/(2.0*(1.0-w)*(y2-y0));
        }
        /*subdivide the curve at t*/
        q = 1.0/(2.0*t*(1.0-t)*(w-1.0)+1.0);
        /*Get position of P6 at t*/
        xx = (t*t*(x0-2.0*w*x1+x2)+2.0*t*(w*x1-x0)+x0)*q;
        yy = (t*t*(y0-2.0*w*y1+y2)+2.0*t*(w*y1-y0)+y0)*q;
        /*Get Squared weight at P5*/
        ww = t*(w-1.0)+1.0; ww *= ww*q;
        /*Get weight of P7*/
        w = ((1.0-t)*(w-1.0)+1.0)*sqrt(q);
        /*Get rounded position of P6*/
        x = floor_float64(xx+0.5); 
        y = floor_float64(yy+0.5);
        /*Get the x position of P5 through the
        intersection of the line through P6 and the line
        between P0-P1*/
        xx = (x1-x0)*(yy-y0)/(y1-y0)+x0;
        OutputDebugStringA("Vertical Tang\n");
        draw_quad_rational_bezier_segment(framebuffer,
                                          x0, y0,
                                          floor_float64(xx+0.5), y,
                                          x, y,
                                          ww, color);
        /*Get x position of P7 from intersection of the
        line through P6 and the line between P1-P2*/
        xx = (x1-x2)*(yy-y2)/(y1-y2)+x2;
        /*P0 = P6 and P1 = P7*/
        x1 = floor_float64(xx+0.5f); x0 = x; y0 = y1 = y;
    }
    
    OutputDebugStringA("Outer\n");
    draw_quad_rational_bezier_segment(framebuffer,
                                      x0, y0,
                                      x1, y1,
                                      x2, y2,
                                      w*w, color);
}

internal void
draw_rotated_ellipse_rect(framebuffer_t *framebuffer,
                          int32 x0, int32 y0,
                          int32 x1, int32 y1,
                          int64 zd, fvector4_t color)
{
    int32 xd = x1-x0, yd = y1-y0;
    float32 w = xd*(int64)yd;
    if(zd == 0)
    {
        return draw_ellipse_rect(framebuffer,
                                 x0, y0, x1, y1,
                                 color);
    }
    if(w != 0.0)
    {
        w = (w-zd)/(w+w);
    }
    ASSERT(w <= 1.0 && w >= 0.0);
    
    xd = floor_float64(xd*w+0.5);
    yd = floor_float64(yd*w+0.5);
    
    draw_quad_rational_bezier_segment(framebuffer,
                                      x0, y0+yd,
                                      x0, y0,
                                      x0+xd, y0,
                                      1.0-w, color);
    draw_quad_rational_bezier_segment(framebuffer,
                                      x0, y0+yd,
                                      x0, y1,
                                      x1-xd, y1,
                                      w, color);
    draw_quad_rational_bezier_segment(framebuffer,
                                      x1, y1-yd,
                                      x1, y1,
                                      x1-xd, y1,
                                      1.0-w, color);
    draw_quad_rational_bezier_segment(framebuffer,
                                      x1, y1-yd,
                                      x1, y0,
                                      x0+xd, y0,
                                      w, color);
}

internal void
draw_rotated_ellipse(framebuffer_t *framebuffer,
                     int32 x, int32 y,
                     int32 a, int32 b,
                     float32 angle, fvector4_t color)
{
    float32 xd = (int64)a*a, yd = (int64)b*b;
    float32 s = sin(angle), zd = (xd-yd)*s;
    xd = sqrt(xd-zd*s);
    yd = sqrt(yd+zd*s);
    a = xd+0.5;
    b = yd+0.5;
    zd = zd*a*b/(xd*yd);
    draw_rotated_ellipse_rect(framebuffer,
                              x-a, y-b,
                              x+a, y+b,
                              (int64)(4*zd*cos(angle)),
                              color);
}

internal void
clear_color(framebuffer_t *framebuffer, fvector4_t color)
{
    uint32 max_width = framebuffer->width;
    uint32 max_height = framebuffer->height;
    
    for(uint32 y = 0; y < max_height; y++)
    {
        for(uint32 x = 0; x < max_width; x++)
        {
            draw_point(framebuffer, x, y, color);
        }
    }
}

internal bool32
is_within_radius(v2_t pos, v2_t center, uint32 radius)
{
    bool32 in_radius = false;
    
    v2_t distance_vector = pos - center;
    float32 distance_sqr = inner_product(distance_vector,
                                         distance_vector);
    
    in_radius = (distance_sqr < radius*radius);
    
    return in_radius;
}


#endif //JUNK_H
