//<LICENSE>

#ifndef MUHRECINTERACTOR_H_
#define MUHRECINTERACTOR_H_
#include <interactors/interactionbase.h>


class MuhrecInteractor: public kipl::interactors::InteractionBase 
{
public:
	MuhrecInteractor();
	virtual ~MuhrecInteractor();
	void Reset()    override;
	void Abort()    override;
	bool Aborted()  override;
	void Done()     override;
	bool Finished() override;

	/// Updates the status of the reconstruction, to be called by the engine and back-projector
	///
	/// Returns true if the process should be aborted.
	bool SetProgress(float progress, std::string msg="") override;

    /// Updates the status of the reconstruction, to be called by the engine and back-projector
    ///
    /// Returns true if the process should be aborted.
    bool SetOverallProgress(float progress) override;

	float CurrentProgress() override;
	std::string CurrentMessage() override;
};

#endif /* MUHRECINTERACTOR_H_ */
