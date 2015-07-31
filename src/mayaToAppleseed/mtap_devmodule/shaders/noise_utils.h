float fTurb(vector point,
            float time,
            float lacunarity,
            float frequencyRatio,
            int octaves[2],
            float ratio,
            vector ripples)
{
   int i = 0;
   float mix = 0.0;
   float amp = 1.0;
   vector pp = vector(pp, ripples[0], ripples[1], ripples[2]);
   pp = point * pp / 2.0;

   float pixelSize = sqrt(area(pp));
   float niquist = 2.0f * pixelSize;
   float pixel = 1.0;

   while ((i < octaves[1] && pixel > niquist) || i < octaves[0])
   {
      AtPoint2 offset;
      AiV2Create(offset, lacunarity, lacunarity);
      mix += amp * fabs(noise((pp + noise(offset)) * lacunarity, time));
      lacunarity *= frequencyRatio;
      amp *= ratio;
      pixel /= frequencyRatio;
      ++i;
   }

   if (pixel > pixelSize && i <= octaves[1])
   {
      vector offset = vector(offset, lacunarity, lacunarity);
      float weight = clamp((pixel/pixelSize - 1.0f), 0.0f, 1.0f);
      mix += weight * amp * fabs(noise((pp+noise(offset)) * lacunarity, time));
   }

   return mix;
}

float fBm(vector p,
          float time,
          float initialAmplitude,
          int octaves[2],
          float initialLacunarity,
          float frequencyRatio,
          float ratio)
{
   float amp = initialAmplitude;
   AtPoint pp = p;
   float sum = 0;
   int i = 0;
   float lacunarity = initialLacunarity;

   // NOTE: this is wrong, sg->area is "world-space" area
   // sg->area is 0.0 for volumetric shaders
   float pixelSize = sqrt(area(pp));
   float nyquist = 2.0f * pixelSize;
   float pixel = 1.0f;

   while ((i < octaves[1] && pixel > nyquist) || i < octaves[0])
   {
      sum += amp * noise(pp * lacunarity, time);
      amp *= ratio;
      lacunarity *= frequencyRatio;
      pixel /= frequencyRatio;
      ++i;
   }

   if (pixel > pixelSize && i <= octaves[1])
   {
      float weight = clamp(pixel/pixelSize - 1.0f, 0.0f, 1.0f);
      sum += weight * amp * noise(pp * lacunarity, time);
   }

   return sum * 0.5f + 0.5f;
}
