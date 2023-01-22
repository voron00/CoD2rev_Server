#pragma once

void DObjInit();
void DObjShutdown();

void XAnimFree(struct XAnimParts *parts);
void XAnimFreeList(struct XAnim_s *anims);

void XModelPartsFree(struct XModelPartsLoad *modelParts);
void XModelFree(struct XModel *model);

void XAnimInit();
void XAnimShutdown();