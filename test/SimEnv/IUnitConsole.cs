namespace SimEnv
{
	public interface IUnitConsole
	{
		IUnitSim Device { get; }
		string Text { get; }
		void Create(string args);
	}
}
