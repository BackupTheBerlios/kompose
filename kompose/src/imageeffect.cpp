//
// C++ Implementation: imageeffect
//
// Description: 
//
//
// Author: Hans Oischinger <oisch@sourceforge.net>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "imageeffect.h"


#include <qimage.h>

ImageEffect::ImageEffect()
{
}


ImageEffect::~ImageEffect()
{
}


void ImageEffect::scale(QImage & src, int new_width, int new_height)
{
    int const old_width = src.width();
    int const old_height = src.height();

    if ( (old_width == new_width) && (old_height == new_height) )
    {
        return;
    }

    QImage dst(new_width, new_height, 32);

    bool const has_alpha = src.hasAlphaBuffer();

    if (has_alpha)
    {
        dst.setAlphaBuffer(true);
    }

    int const scale = 4096;

    static int last_old_width = -1;
    static int last_old_height = -1;
    static int last_new_width = -1;
    static int last_new_height = -1;

    static std::vector<int> col_lengths;
    static std::vector<int> col_starts;
    static std::vector<int> col_factor_offsets;
    static std::vector<Q_UINT32> col_factors;

    static std::vector<int> row_lengths;
    static std::vector<int> row_starts;
    static std::vector<int> row_factor_offsets;
    static std::vector<Q_UINT32> row_factors;

    if ( (last_old_height != old_height) || (last_new_height != new_height) )
    {
        calcScaleFactors(old_height, new_height, scale, row_lengths,
                         row_starts, row_factor_offsets, row_factors);

        last_old_height = old_height;
        last_new_height = new_height;
    }

    if ( (last_old_width != old_width) || (last_new_width != new_width) )
    {
        calcScaleFactors(old_width, new_width, scale, col_lengths,
                         col_starts, col_factor_offsets, col_factors);

        last_old_width = old_width;
        last_new_width = new_width;
    }

    if (has_alpha)
    {
        std::vector<Q_UINT32> temp_red(old_width);
        std::vector<Q_UINT32> temp_green(old_width);
        std::vector<Q_UINT32> temp_blue(old_width);
        std::vector<Q_UINT32> temp_alpha(old_width);

        for (int y = 0; y < new_height; ++y)
        {
            for (int x = 0; x < old_width; ++x)
            {
                temp_red[x] = 0;
                temp_green[x] = 0;
                temp_blue[x] = 0;
                temp_alpha[x] = 0;
            }

            int const from_y2 = row_starts[y];
            int const to_y2 = from_y2 + row_lengths[y];
            int row_factor_offset = row_factor_offsets[y];

            for (int y2 = from_y2; y2 < to_y2; ++y2)
            {
                QRgb const * scanline = reinterpret_cast<QRgb const *> (src.scanLine(y2));

                Q_UINT32 const factor = row_factors[row_factor_offset];
                ++row_factor_offset;

                for (int x = 0; x < old_width; ++x)
                {
                    QRgb const pixel = scanline[x];
                    Q_UINT32 const act_alpha = qAlpha(pixel);

                    if (act_alpha)
                    {
                        Q_UINT32 const alpha_factor = act_alpha * factor;

                        temp_red[x] += (qRed(pixel) * alpha_factor) >> 8;
                        temp_green[x] += (qGreen(pixel) * alpha_factor) >> 8;
                        temp_blue[x] += (qBlue(pixel) * alpha_factor) >> 8;
                        temp_alpha[x] += alpha_factor;
                    }
                }
            }

            QRgb * scanline = reinterpret_cast<QRgb *> (dst.scanLine(y));

            for (int x = 0; x < new_width; ++x)
            {
                int const from_x2 = col_starts[x];
                int const to_x2 = from_x2 + col_lengths[x];
                int col_factor_offset = col_factor_offsets[x];

                Q_UINT32 red = 0;
                Q_UINT32 green = 0;
                Q_UINT32 blue = 0;
                Q_UINT32 alpha = 0;

                for (int x2 = from_x2; x2 < to_x2; ++x2)
                {
                    Q_UINT32 const factor = col_factors[col_factor_offset];
                    ++col_factor_offset;

                    Q_UINT32 const act_alpha = temp_alpha[x2];

                    if (act_alpha)
                    {
                        red += temp_red[x2] * factor;
                        green += temp_green[x2] * factor;
                        blue += temp_blue[x2] * factor;
                        alpha += act_alpha * factor;
                    }
                }

                alpha >>= 8;

                if (alpha)
                {
                    scanline[x] = qRgba(red / alpha, green / alpha, blue / alpha, alpha >> 16);
                }
                else
                {
                    scanline[x] = qRgba(255, 255, 255, 0);
                }
            }
        }
    }
    else
    {
        std::vector<Q_UINT32> temp_red(old_width);
        std::vector<Q_UINT32> temp_green(old_width);
        std::vector<Q_UINT32> temp_blue(old_width);

        for (int y = 0; y < new_height; ++y)
        {
            int const from_y2 = row_starts[y];
            int const to_y2 = from_y2 + row_lengths[y];
            int row_factor_offset = row_factor_offsets[y];

            for (int x = 0; x < old_width; ++x)
            {
                temp_red[x] = 0;
                temp_green[x] = 0;
                temp_blue[x] = 0;
            }

            for (int y2 = from_y2; y2 < to_y2; ++y2)
            {
                QRgb const * scanline = reinterpret_cast<QRgb *> (src.scanLine(y2));

                Q_UINT32 const factor = row_factors[row_factor_offset];
                ++row_factor_offset;

                for (int x = 0; x < old_width; ++x)
                {
                    QRgb const pixel = scanline[x];

                    temp_red[x] += qRed(pixel) * factor;
                    temp_green[x] += qGreen(pixel) * factor;
                    temp_blue[x] += qBlue(pixel) * factor;
                }
            }

            QRgb * scanline = reinterpret_cast<QRgb *> (dst.scanLine(y));

            for (int x = 0; x < new_width; ++x)
            {
                int const from_x2 = col_starts[x];
                int const to_x2 = from_x2 + col_lengths[x];
                int col_factor_offset = col_factor_offsets[x];

                Q_UINT32 red = 0;
                Q_UINT32 green = 0;
                Q_UINT32 blue = 0;

                for (int x2 = from_x2; x2 < to_x2; ++x2)
                {
                    Q_UINT32 const factor = col_factors[col_factor_offset];
                    ++col_factor_offset;

                    red += temp_red[x2] * factor;
                    green += temp_green[x2] * factor;
                    blue += temp_blue[x2] * factor;
                }

                scanline[x] = qRgb(red >> 24, green >> 24, blue >> 24);
            }
        }
    }

    src = dst;
}


void ImageEffect::calcScaleFactors(int from_pixels, int to_pixels, int sum, std::vector<int> & lengths,
                                   std::vector<int> & starts, std::vector<int> & factor_offsets,
                                   std::vector<Q_UINT32> & factors)
{
    int const div = from_pixels / to_pixels;
    int const rem = from_pixels - div * to_pixels;
    int const sum_sum = rem + div * to_pixels;
    int const sum_div = sum / sum_sum;
    int const sum_rem = sum - sum_div * sum_sum;
    int part_div = 0;
    int part_rem = 0;

    lengths.resize(to_pixels);
    starts.resize(to_pixels);
    factor_offsets.resize(to_pixels);
    factors.resize(0);

    for (int i = 0; i < to_pixels; ++i)
    {
        int const from_index = part_div;
        int const from_rem = part_rem;

        part_rem += rem;

        if (part_rem >= to_pixels)
        {
            part_rem -= to_pixels;
            ++part_div;
        }

        part_div += div;

        int to_index = part_div;
        int to_rem = part_rem - 1;

        if (to_rem < 0)
        {
            to_rem = to_pixels - 1;
            --to_index;
        }

        int const length = to_index - from_index + 1;

        lengths[i] = length;
        starts[i] = from_index;
        factor_offsets[i] = static_cast<int> (factors.size());

        if (length == 1)
        {
            factors.push_back(sum);
        }
        else
        {
            int sum_part_div = 0;
            int sum_part_rem = 0;

            for (int j = 0; j < length; ++j)
            {
                int act_rem = to_pixels;

                if (j == 0)
                {
                    act_rem = to_pixels - from_rem;
                }
                else if (j == length - 1)
                {
                    act_rem = to_rem + 1;
                }

                int new_sum_part_rem = sum_part_rem + act_rem * sum_rem;
                int const new_sum_part_rem_div = new_sum_part_rem / sum_sum;
                new_sum_part_rem -= new_sum_part_rem_div * sum_sum;
                int const new_sum_part_div = sum_part_div + act_rem * sum_div + new_sum_part_rem_div;

                factors.push_back(new_sum_part_div - sum_part_div);

                sum_part_div = new_sum_part_div;
                sum_part_rem = new_sum_part_rem;
            }
        }
    }
}
