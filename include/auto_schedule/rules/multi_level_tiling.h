#ifndef FREE_TENSOR_MULTI_LEVEL_TILING_H
#define FREE_TENSOR_MULTI_LEVEL_TILING_H

#include <array>
#include <utility>

#include <analyze/find_multi_level_tiling.h>
#include <auto_schedule/rule.h>
#include <auto_schedule/sketch.h>
#include <auto_schedule/structs.h>

namespace freetensor {

class MultiLevelTilingRule : public Rule {
    std::string pat_;

  public:
    explicit MultiLevelTilingRule(TargetType target) {
        if (target == TargetType::CPU) {
            pat_ = "SSRSRS";
        } else {
            pat_ = "SSSRRSRS";
        }
    }
    RuleStatus analyze(const Sketch &sketch) override;
    std::vector<Ref<Sketch>> genPart(const Sketch &sketch) override;
};

class MultiLevelTilingPart : public SketchPartNode {
  protected:
    ForsWithDataReuse target_;
    MultiLevelTilingAnnotation annotation_;
    std::string pat_;
    int spaceLoopTimes_;
    int frontSpaceLoopTimes_;
    int reductionLoopTimes_;
    std::vector<std::pair<ID, int>> tiles_;

  public:
    void genRandAnnotation(RNG &gen) override;
    void genFakeAnnotation(RNG &gen) override;
    std::vector<std::pair<ID, int>> &tiles() { return tiles_; }
    explicit MultiLevelTilingPart(ForsWithDataReuse fors,
                                  std::string pat = "SSRSRS");
    void apply(Schedule &schedule, SubSketch &subSketch) override;
    bool mutate(RNG &gen) override;
    bool crossover(const SketchPart &part, RNG &gen) override;
    [[nodiscard]] std::vector<int> getAnnotation() const override;
    size_t spaceLoopLength() const { return target_.spaceLoops.size(); }
    size_t frontSpaceLoopTimes() const { return frontSpaceLoopTimes_; }
    [[nodiscard]] size_t hash() const override;
    SketchPartType partType() override {
        return SketchPartType::MultiLevelTiling;
    }
    [[nodiscard]] SketchPart clone() const override {
        return Ref<MultiLevelTilingPart>::make(*this);
    }
    void setAnnotation(MultiLevelTilingAnnotation anno) {
        annotation_ = std::move(anno);
    }
};

} // namespace freetensor

#endif // FREE_TENSOR_MULTI_LEVEL_TILING_H
