package wits.web;

import java.util.Map;

public interface DataMap {

	/**
	 * Put some data in a map
	 *
	 * @param key
	 *            the key for the data
	 * @param obj
	 *            the data
	 * @return a reference to this, so the API can be used fluently
	 */
	DataMap put(String key, Object obj);

	/**
	 * Get some data from the map
	 *
	 * @param key
	 *            the key of the data
	 * @return the data
	 */
	<T> T get(String key);

	/**
	 * Remove some data from the map
	 *
	 * @param key
	 *            the key of the data
	 * @return the data that was there or null if none there
	 */
	<T> T remove(String key);

	/**
	 * @return the data as a map
	 */
	Map<String, Object> data();

}
