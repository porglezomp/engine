BLEND_DIR=assets/blender
MODEL_DIR=assets/models
BLENDS=$(wildcard $(BLEND_DIR)/*.blend)
MODELS=$(patsubst $(BLEND_DIR)/%.blend,$(MODEL_DIR)/%.model,$(BLENDS))
BLENDER=blender

all: $(MODELS)
	echo $(MODELS)

$(MODEL_DIR)/%.model: $(BLEND_DIR)/%.blend
	$(BLENDER) --background $< --python export.py -- $@

