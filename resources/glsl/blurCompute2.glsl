//$ -- Blur constants
//$ BLUR_SIZE = config.render.effects.blur_size * 2 + 1    
//$ BLUR_SIGMA = 0.3                 -- Sigma from gaussian function
//$ BLUR_PIXEL = 2                   -- Sample every Nth pixel (ideally, 1)
//$ BLUR_WEIGHTS = {}
//$
//$ function gaussian(x,sigma)
//$  return math.exp(-x*x / (2 * sigma * sigma)) / (math.sqrt(2 * math.pi) * sigma)
//$ end
//$ -- Init gaussian weights
//$ do
//$  local sum = 0
//$  -- Compute weights
//$  for i=0, BLUR_SIZE-1 do
//$    local x = i / (BLUR_SIZE - 1) - 0.5
//$    local g = gaussian(x, BLUR_SIGMA)
//$    BLUR_WEIGHTS[i+1] = g
//$    sum = sum + g
//$  end
//$  -- Normalize
//$  for i=1, BLUR_SIZE do
//$    BLUR_WEIGHTS[i] = BLUR_WEIGHTS[i] / sum
//$  end
//$ end

void main()
{
  vec4 sum = vec4(0.0);
  float blurSize = 1.0f;

  //$ for i=1, BLUR_SIZE do
  //$ local OFFSET = (i - BLUR_SIZE/2 - 0.5) * BLUR_PIXEL / RENDER_WIDTH
  //$ if SCENE_PASS_BLURX then
      sum += texture2D(tex_scene, vec2(v_texcoord.x + ($(OFFSET) * blurSize), v_texcoord.y)) * $(BLUR_WEIGHTS[i]);
  //$ elseif SCENE_PASS_BLURY then
      sum += texture2D(tex_scene, vec2(v_texcoord.x, v_texcoord.y + ($(OFFSET) * blurSize))) * $(BLUR_WEIGHTS[i]);
  //$ end
  //$ end
  gl_FragColor = sum;
}
